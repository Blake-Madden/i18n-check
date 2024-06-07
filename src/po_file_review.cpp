#include "po_file_review.h"

namespace i18n_check
    {
    //------------------------------------------------
    void po_file_review::operator()(std::wstring_view poFileText, const std::wstring& fileName)
        {
        m_file_name = fileName;

        if (poFileText.empty())
            {
            return;
            }

        static const std::wstring_view MSGID{ L"msgid \"" };
        static const std::wstring_view MSGID_PLURAL{ L"msgid_plural \"" };
        static const std::wstring_view MSGSTR{ L"msgstr \"" };
        static const std::wstring_view MSGSTR0{ L"msgstr[0] \"" };
        static const std::wstring_view MSGSTR1{ L"msgstr[1] \"" };
        // type of printf formatting the string uses
        static const std::wstring_view CFORMAT{ L"#, c-format" };
        static const std::wstring_view CPPFORMAT{ L"#, cpp-format" };

        while (!poFileText.empty())
            {
            auto [entryFound, entry] = read_catalog_entry(poFileText);
            if (!entryFound)
                {
                break;
                }
            // step over the section
            poFileText.remove_prefix(entry.length());

            po_format_string pofs{ po_format_string::no_format };
            if (entry.find(CFORMAT) != std::wstring::npos ||
                entry.find(CPPFORMAT) != std::wstring::npos)
                {
                pofs = po_format_string::ccpp_format;
                }

            // read section from catalog entry
            auto [msgIdFound, msgId] = read_msg(entry, MSGID);
            if (!msgIdFound)
                {
                continue;
                }
            // plural is optional, translations may be one string or two (for plural)
            auto [msgIdPluralFound, msgPluralId] = read_msg(entry, MSGID_PLURAL);
            auto [msgStrFound, msgStr] = read_msg(entry, MSGSTR);
            auto [msgStr0Found, msg0Str] = read_msg(entry, MSGSTR0);
            auto [msgStr1Found, msg1Str] = read_msg(entry, MSGSTR1);

            m_catalog_entries.push_back(std::make_pair(
                fileName, po_catalog_entry{ std::move(msgId), std::move(msgPluralId),
                                            // when there is a plural, then msgstr 0-1 is where the
                                            // singular nd plural translations are kept
                                            msgStr.empty() ? std::move(msg0Str) : std::move(msgStr),
                                            std::move(msg1Str), pofs }));
            }

        if (static_cast<bool>(m_reviewStyles & check_mismatching_printf_commands))
            {
            review_prinf_issues();
            }
        }

    //------------------------------------------------
    std::pair<bool, std::wstring_view>
    po_file_review::read_catalog_entry(std::wstring_view& poFileText)
        {
        size_t entryPos = poFileText.find(L"\n#");
        if (entryPos == std::wstring_view::npos)
            {
            return std::make_pair(false, std::wstring_view{});
            }
        poFileText.remove_prefix(entryPos);

        /// @todo handle \r
        size_t endOfEntryPos = poFileText.find(L"\n\n");
        if (endOfEntryPos == std::wstring_view::npos)
            {
            return std::make_pair(true, poFileText);
            }
        return std::make_pair(true, poFileText.substr(0, endOfEntryPos));
        }

    //------------------------------------------------
    std::pair<bool, std::wstring> po_file_review::read_msg(std::wstring_view& poFileText,
                                                           const std::wstring_view msgTag)
        {
        size_t idPos = poFileText.find(msgTag);
        if (idPos == std::wstring_view::npos)
            {
            return std::make_pair(false, std::wstring{});
            }
        poFileText.remove_prefix(idPos + msgTag.length());

        size_t idEndPos{ 0 };
        while (true)
            {
            idEndPos = poFileText.find(L'\"', idEndPos);
            if (idEndPos == std::wstring_view::npos)
                {
                return std::make_pair(false, std::wstring{});
                }
            // skip escaped quotes
            if (idEndPos > 0 && poFileText[idEndPos - 1] == L'\\')
                {
                ++idEndPos;
                continue;
                }
            else
                {
                size_t lookAheadIndex{ idEndPos + 1 };
                while (lookAheadIndex < poFileText.length() &&
                       string_util::is_either(poFileText[lookAheadIndex], L'\r', L'\n'))
                    {
                    ++lookAheadIndex;
                    }
                if (lookAheadIndex < poFileText.length() && poFileText[lookAheadIndex] == L'"')
                    {
                    idEndPos = lookAheadIndex + 1;
                    continue;
                    }
                break;
                }
            }
        std::wstring msgId{ poFileText.substr(0, idEndPos) };
        if (msgId.length() > 0 && msgId.front() == L'"')
            {
            msgId.erase(0, 1);
            }
        /// @todo make single pass
        string_util::replace_all<std::wstring>(msgId, L"\"\r\n\"", L"");
        string_util::replace_all<std::wstring>(msgId, L"\r\n\"", L"");
        string_util::replace_all<std::wstring>(msgId, L"\"\n\"", L"");
        string_util::replace_all<std::wstring>(msgId, L"\n\"", L"");
        poFileText.remove_prefix(idEndPos);

        return std::make_pair(true, msgId);
        }

    //------------------------------------------------
    void po_file_review::review_prinf_issues()
        {
        std::vector<std::wstring> printfStrings1, printfStrings2;
        for (auto& catEntry : m_catalog_entries)
            {
            if (catEntry.second.m_po_format == po_format_string::ccpp_format)
                {
                // only look at strings that have a translation
                if (!catEntry.second.m_translation.empty())
                    {
                    printfStrings1.clear();
                    printfStrings2.clear();
                    std::copy(std::regex_token_iterator<
                                  decltype(catEntry.second.m_source)::const_iterator>(
                                  catEntry.second.m_source.cbegin(),
                                  catEntry.second.m_source.cend(), m_printfCppRE),
                              std::regex_token_iterator<
                                  decltype(catEntry.second.m_source)::const_iterator>{},
                              std::back_inserter(printfStrings1));
                    std::copy(std::regex_token_iterator<
                                  decltype(catEntry.second.m_translation)::const_iterator>(
                                  catEntry.second.m_translation.cbegin(),
                                  catEntry.second.m_translation.cend(), m_printfCppRE),
                              std::regex_token_iterator<
                                  decltype(catEntry.second.m_translation)::const_iterator>{},
                              std::back_inserter(printfStrings2));

                    if (printfStrings1.size() || printfStrings2.size())
                        {
                        if (printfStrings1 != printfStrings2)
                            {
                            catEntry.second.m_issues.emplace_back(
                                translation_issue::printf_issue,
                                L"\"" + catEntry.second.m_source + L"\" vs. \"" +
                                    catEntry.second.m_translation + L"\"");
                            }
                        }
                    }

                if (!catEntry.second.m_translation_plural.empty())
                    {
                    printfStrings1.clear();
                    printfStrings2.clear();
                    std::copy(std::regex_token_iterator<
                                  decltype(catEntry.second.m_source_plural)::const_iterator>(
                                  catEntry.second.m_source_plural.cbegin(),
                                  catEntry.second.m_source_plural.cend(), m_printfCppRE),
                              std::regex_token_iterator<
                                  decltype(catEntry.second.m_source_plural)::const_iterator>{},
                              std::back_inserter(printfStrings1));
                    std::copy(std::regex_token_iterator<
                                  decltype(catEntry.second.m_translation_plural)::const_iterator>(
                                  catEntry.second.m_translation_plural.cbegin(),
                                  catEntry.second.m_translation_plural.cend(), m_printfCppRE),
                              std::regex_token_iterator<
                                  decltype(catEntry.second.m_translation_plural)::const_iterator>{},
                              std::back_inserter(printfStrings2));

                    if (printfStrings1.size() || printfStrings2.size())
                        {
                        if (printfStrings1 != printfStrings2)
                            {
                            catEntry.second.m_issues.emplace_back(
                                translation_issue::printf_issue,
                                L"\"" + catEntry.second.m_source_plural + L"\" vs. \"" +
                                    catEntry.second.m_translation_plural + L"\"");
                            }
                        }
                    }
                }
            }
        }
    } // namespace i18n_check
