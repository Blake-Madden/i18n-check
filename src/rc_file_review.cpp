#include "rc_file_review.h"

using namespace i18n_check;

void rc_file_review::operator()(const std::wstring_view rcFileText,
    const std::wstring& file_name /*= L""*/)
    {
    m_file_name = file_name;

    if (rcFileText.length() == 0)
        { return; }

    /* This regex:

       STRINGTABLE[[:space:]]*(BEGIN|\{)[\n\r]*([[:space:]]*[A-Za-z0-9_]+[,]?[[:space:]]*[L]?"[^\n\r]*[\n\r]*)+(END|\}))
       
       can find a full string table, but causes an error_stack exception with std::regex with some files,
       so need to more crudely parse string table by looking for the start and end tags.*/
    std::vector<std::wstring> stringTables;
    const std::wregex STRINGTABLE{ LR"(STRINGTABLE[[:space:]]*(BEGIN|\{)[[:space:]]*)" };
    const std::wregex END{ LR"([\r\n]+(END|\}))" };
    std::match_results<decltype(rcFileText)::const_iterator> stPostions;
    std::match_results<decltype(rcFileText)::const_iterator> endPostions;
    auto currentPos{ rcFileText };
    while (std::regex_search(currentPos.cbegin(), currentPos.cend(), stPostions, STRINGTABLE))
        {
        currentPos = currentPos.substr(stPostions.position());
        if (std::regex_search(currentPos.cbegin(), currentPos.cend(), endPostions, END))
            {
            stringTables.push_back(
                std::wstring(currentPos.substr(0, endPostions.position() + endPostions.length())));

            currentPos = currentPos.substr(endPostions.position() + endPostions.length());
            }
        }

    std::vector<std::wstring> tableEntries;
    const std::wregex tableEntryRE{ LR"("([^\n\r]*))" };
    for (const auto& sTab : stringTables)
        {
        std::copy(
            std::regex_token_iterator<std::remove_reference_t<decltype(sTab)>::const_iterator>(
                sTab.cbegin(), sTab.cend(), tableEntryRE, 1),
            std::regex_token_iterator<std::remove_reference_t<decltype(sTab)>::const_iterator>{},
            std::back_inserter(tableEntries));
        }

    // review table entries
    for (auto& te : tableEntries)
        {
        // strip off trailing quote
        if (te.length() > 0)
            { te.pop_back(); }
        const auto originalStr{ te }; // don't include transformed string in report
        if (is_untranslatable_string(te, false))
            {
            m_unsafe_localizable_strings.push_back(
                string_info(originalStr,
                    string_info::usage_info(
                        string_info::usage_info::usage_type::orphan,
                        std::wstring{}, std::wstring{}), m_file_name,
                    std::make_pair(-1, -1)));
            }
        else
            {
            m_localizable_strings.push_back(
                string_info(originalStr,
                    string_info::usage_info(
                        string_info::usage_info::usage_type::orphan,
                        std::wstring{}, std::wstring{}), m_file_name,
                    std::make_pair(-1, -1)));
            }
        }
    }
