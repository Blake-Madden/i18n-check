/********************************************************************************
 * Copyright (c) 2023-2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#include "rc_file_review.h"

namespace i18n_check
    {
    void rc_file_review::operator()(const std::wstring_view rcFileText,
                                    const std::filesystem::path& fileName)
        {
        m_file_name = fileName;

        if (rcFileText.empty())
            {
            return;
            }

        if (static_cast<bool>(get_style() & check_l10n_strings))
            {
            /* This regex:

               STRINGTABLE[[:space:]]*(BEGIN|\{)[\n\r]*([[:space:]]*[A-Za-z0-9_]+[,]?[[:space:]]*[L]?"[^\n\r]*[\n\r]*)+(END|\}))

               can find a full string table, but causes an error_stack exception with std::regex
               with some files, so need to more crudely parse string tables by looking for the start
               and end tags.*/
            std::vector<std::pair<size_t, std::wstring>> stringTables;
            const std::wregex stringTableRegEx{
                LR"(STRINGTABLE[[:space:]]*(BEGIN|\{)[[:space:]]*)"
            };
            const std::wregex stringTableEndRegEx{ LR"([\r\n]+[[:space:]]*(END|\}))" };
            std::match_results<decltype(rcFileText)::const_iterator> stPositions;
            std::match_results<decltype(rcFileText)::const_iterator> endPositions;
            auto currentTextBlock{ rcFileText };
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, stringTableRegEx))
                {
                currentBlockOffset += stPositions.position();
                currentTextBlock = currentTextBlock.substr(stPositions.position());
                if (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                      endPositions, stringTableEndRegEx))
                    {
                    stringTables.emplace_back(
                        currentBlockOffset, currentTextBlock.substr(0, endPositions.position() +
                                                                           endPositions.length()));

                    currentBlockOffset += endPositions.length();

                    currentTextBlock = currentTextBlock.substr(endPositions.length());
                    }
                else
                    {
                    break;
                    }
                }

            std::vector<std::pair<size_t, std::wstring>> tableEntries;
            const std::wregex tableEntryRE{ LR"("([^\n\r]*))" };
            for (std::pair<size_t, std::wstring> sTab : stringTables)
                {
                size_t matchLenghtOffset{ 0 };
                std::wsmatch tabEntryResults;
                while (std::regex_search(sTab.second, tabEntryResults, tableEntryRE))
                    {
                    tableEntries.emplace_back(sTab.first + tabEntryResults.position(1) +
                                                  matchLenghtOffset,
                                              tabEntryResults.str(1));
                    sTab.second.erase(0, tabEntryResults.position(1) + tabEntryResults.length(1));
                    matchLenghtOffset += tabEntryResults.position(1) + tabEntryResults.length(1);
                    }
                }

            // review table entries
            for (auto& tableEntry : tableEntries)
                {
                // strip off trailing quote
                if (tableEntry.second.length() > 0)
                    {
                    tableEntry.second.pop_back();
                    }
                if (is_untranslatable_string(tableEntry.second, false))
                    {
                    m_unsafe_localizable_strings.emplace_back(
                        tableEntry.second,
                        string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                std::wstring{}, std::wstring{}),
                        m_file_name,
                        std::make_pair(get_line_and_column(tableEntry.first, rcFileText).first,
                                       std::wstring::npos));
                    }
                else
                    {
                    m_localizable_strings.emplace_back(
                        tableEntry.second,
                        string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                std::wstring{}, std::wstring{}),
                        m_file_name,
                        std::make_pair(get_line_and_column(tableEntry.first, rcFileText).first,
                                       std::wstring::npos));
                    }

                if (m_review_styles & check_l10n_contains_url)
                    {
                    std::wsmatch results;
                    if (std::regex_search(tableEntry.second, results, m_url_email_regex))
                        {
                        m_localizable_strings_with_urls.emplace_back(
                            tableEntry.second,
                            string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                    std::wstring{}, std::wstring{}),
                            m_file_name,
                            std::make_pair(get_line_and_column(tableEntry.first, rcFileText).first,
                                           std::wstring::npos));
                        }
                    }

#if __cplusplus >= 202002L
                if ((m_review_styles & check_l10n_has_surrounding_spaces) &&
                    has_surrounding_spaces(tableEntry.second))
                    {
                    m_localizable_strings_with_surrounding_spaces.emplace_back(
                        tableEntry.second,
                        string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                std::wstring{}, std::wstring{}),
                        m_file_name,
                        std::make_pair(get_line_and_column(tableEntry.first, rcFileText).first,
                                       std::wstring::npos));
                    }
#endif
                }
            }

        // read the fonts
        if (static_cast<bool>(get_style() & check_fonts))
            {
            std::vector<std::pair<size_t, std::wstring>> fontEntries;
            const std::wregex fontRE{ L"\\bFONT[ ]*([0-9]+),[ ]*\"([^\"]*)\"" };
            std::match_results<decltype(rcFileText)::const_iterator> stPositions;
            auto currentTextBlock{ rcFileText };
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, fontRE))
                {
                currentBlockOffset += stPositions.position();
                currentTextBlock = currentTextBlock.substr(stPositions.position());

                fontEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(0, stPositions.position() + stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock = currentTextBlock.substr(stPositions.length());
                }

            std::vector<std::wstring> fontParts;
            for (const auto& fontEntry : fontEntries)
                {
                fontParts.clear();
                auto theFontEntry{ fontEntry.second };
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(theFontEntry)>::const_iterator>(
                              theFontEntry.cbegin(), theFontEntry.cend(), fontRE, { 1, 2 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(theFontEntry)>::const_iterator>{},
                          std::back_inserter(fontParts));

                const auto fontSize = [&fontParts]()
                {
                    try
                        {
                        if (fontParts[0].starts_with(L"0x"))
                            {
                            return std::optional<int32_t>(std::stol(fontParts[0], nullptr, 16));
                            }
                        return std::optional<int32_t>(std::stol(fontParts[0]));
                        }
                    catch (...)
                        {
                        return std::optional<int32_t>{ std::nullopt };
                        }
                }();

                // 8 is the standard size, but accept up to 10
                constexpr int32_t minFontSize{ 8 };
                constexpr int32_t maxFontSize{ 10 };
                if (fontSize && (fontSize.value() > maxFontSize || fontSize.value() < minFontSize))
                    {
                    m_badFontSizes.push_back(string_info{
                        std::to_wstring(fontSize.value()),
                        string_info::usage_info{
#ifdef wxVERSION_NUMBER
                            wxString::Format(_(L"Font size %s is non-standard (8 is recommended)."),
                                             fontParts[0])
                                .wc_string()
#else
                            _DT(L"Font size ") + fontParts[0] +
                            _DT(L" is non-standard (8 is recommended).")
#endif
                        },
                        fileName,
                        std::make_pair(get_line_and_column(fontEntry.first, rcFileText).first,
                                       std::wstring::npos) });
                    }

                if (fontParts[1] != L"MS Shell Dlg" && fontParts[1] != L"MS Shell Dlg 2")
                    {
                    m_nonSystemFontNames.push_back(string_info{
                        fontParts[1],
#ifdef wxVERSION_NUMBER
                        string_info::usage_info{
                            wxString::Format(_(L"Font '%s' may not map well on some systems "
                                               "(MS Shell Dlg is recommended)."),
                                             fontParts[1])
                                .wc_string() },
#else
                        string_info::usage_info{ _DT(L"Font '") + fontParts[1] +
                                                 _DT(L"' may not map well on some systems (MS "
                                                     "Shell Dlg is recommended).") },
#endif
                        fileName,
                        std::make_pair(get_line_and_column(fontEntry.first, rcFileText).first,
                                       std::wstring::npos) });
                    }
                }
            }
        }
    } // namespace i18n_check
