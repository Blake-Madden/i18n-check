///////////////////////////////////////////////////////////////////////////////
// Name:        rc_file_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "rc_file_review.h"

namespace i18n_check
    {
    void rc_file_review::operator()(const std::wstring_view rcFileText,
                                    const std::wstring& fileName)
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
            const std::wregex stringTableEndRegEx{ LR"([\r\n]+(END|\}))" };
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
                                       -1));
                    }
                else
                    {
                    m_localizable_strings.emplace_back(
                        tableEntry.second,
                        string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                std::wstring{}, std::wstring{}),
                        m_file_name,
                        std::make_pair(get_line_and_column(tableEntry.first, rcFileText).first,
                                       -1));
                    }

                if (m_reviewStyles & check_l10n_contains_url)
                    {
                    std::wsmatch results;

                    if (std::regex_search(tableEntry.second, results, m_urlEmailRE))
                        {
                        m_localizable_strings_with_urls.emplace_back(
                            tableEntry.second,
                            string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                    std::wstring{}, std::wstring{}),
                            m_file_name,
                            std::make_pair(get_line_and_column(tableEntry.first, rcFileText).first,
                                           -1));
                        }
                    }
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
                        string_info::usage_info{ L"Font size " + fontParts[0] +
                                                 L" is non-standard (8 is recommended)." },
                        fileName,
                        std::make_pair(get_line_and_column(fontEntry.first, rcFileText).first,
                                       std::wstring::npos) });
                    }

                if (fontParts[1] != L"MS Shell Dlg" && fontParts[1] != L"MS Shell Dlg 2")
                    {
                    m_nonSystemFontNames.push_back(string_info{
                        fontParts[1],
                        string_info::usage_info{
                            L"Font '" + fontParts[1] +
                            L"' may not map well on some systems (MS Shell Dlg is recommended)." },
                        fileName,
                        std::make_pair(get_line_and_column(fontEntry.first, rcFileText).first,
                                       std::wstring::npos) });
                    }
                }
            }
        }
    } // namespace i18n_check
