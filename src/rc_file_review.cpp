#include "rc_file_review.h"

namespace i18n_check
    {
    void rc_file_review::operator()(const std::wstring_view rcFileText,
                                    const std::wstring& fileName)
        {
        m_file_name = fileName;

        if (rcFileText.length() == 0)
            {
            return;
            }

        if (static_cast<bool>(get_style() & check_l10n_strings))
            {
            /* This regex:

               STRINGTABLE[[:space:]]*(BEGIN|\{)[\n\r]*([[:space:]]*[A-Za-z0-9_]+[,]?[[:space:]]*[L]?"[^\n\r]*[\n\r]*)+(END|\}))

               can find a full string table, but causes an error_stack exception with std::regex
               with some files, so need to more crudely parse string table by looking for the start
               and end tags.*/
            std::vector<std::wstring> stringTables;
            const std::wregex stringTableRegEx{
                LR"(STRINGTABLE[[:space:]]*(BEGIN|\{)[[:space:]]*)"
            };
            const std::wregex stringTableEndRegEx{ LR"([\r\n]+(END|\}))" };
            std::match_results<decltype(rcFileText)::const_iterator> stPositions;
            std::match_results<decltype(rcFileText)::const_iterator> endPositions;
            auto currentPos{ rcFileText };
            while (std::regex_search(currentPos.cbegin(), currentPos.cend(), stPositions,
                                     stringTableRegEx))
                {
                currentPos = currentPos.substr(stPositions.position());
                if (std::regex_search(currentPos.cbegin(), currentPos.cend(), endPositions,
                                      stringTableEndRegEx))
                    {
                    stringTables.emplace_back(
                        currentPos.substr(0, endPositions.position() + endPositions.length()));

                    currentPos = currentPos.substr(endPositions.position() + endPositions.length());
                    }
                }

            std::vector<std::wstring> tableEntries;
            const std::wregex tableEntryRE{ LR"("([^\n\r]*))" };
            for (const auto& sTab : stringTables)
                {
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(sTab)>::const_iterator>(
                              sTab.cbegin(), sTab.cend(), tableEntryRE, 1),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(sTab)>::const_iterator>{},
                          std::back_inserter(tableEntries));
                }

            // review table entries
            for (auto& tabEntry : tableEntries)
                {
                // strip off trailing quote
                if (tabEntry.length() > 0)
                    {
                    tabEntry.pop_back();
                    }
                // don't include transformed string in report
                const auto originalStr{ tabEntry };
                if (is_untranslatable_string(tabEntry, false))
                    {
                    m_unsafe_localizable_strings.emplace_back(
                        originalStr,
                        string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                std::wstring{}, std::wstring{}),
                        m_file_name, std::make_pair(-1, -1));
                    }
                else
                    {
                    m_localizable_strings.emplace_back(
                        originalStr,
                        string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                std::wstring{}, std::wstring{}),
                        m_file_name, std::make_pair(-1, -1));
                    }
                }
            }

        // read the fonts
        if (static_cast<bool>(get_style() & check_fonts))
            {
            std::vector<std::wstring> fontEntries;
            std::vector<std::wstring> fontParts;
            const std::wregex fontRE{ L"\\bFONT[ ]*([0-9]+),[ ]*\"([^\"]*)\"" };
            std::copy(std::regex_token_iterator<decltype(rcFileText)::const_iterator>(
                          rcFileText.cbegin(), rcFileText.cend(), fontRE),
                      std::regex_token_iterator<decltype(rcFileText)::const_iterator>{},
                      std::back_inserter(fontEntries));

            for (const auto& fontEntry : fontEntries)
                {
                fontParts.clear();
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(fontEntry)>::const_iterator>(
                              fontEntry.cbegin(), fontEntry.cend(), fontRE, { 1, 2 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(fontEntry)>::const_iterator>{},
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
                        fontEntry + L": font size " + fontParts[0] +
                            L" is non-standard (8 is recommended).",
                        string_info::usage_info{}, fileName, std::make_pair(-1, -1) });
                    }

                if (fontParts[1] != L"MS Shell Dlg" && fontParts[1] != L"MS Shell Dlg 2")
                    {
                    m_nonSystemFontNames.push_back(string_info{
                        fontEntry + L": font name '" + fontParts[1] +
                            L"' may not map well on some systems (MS Shell Dlg is recommended).",
                        string_info::usage_info{}, fileName, std::make_pair(-1, -1) });
                    }
                }
            }
        }
    } // namespace i18n_check
