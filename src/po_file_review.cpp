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
        static const std::wregex entryLineRegEx{ LR"(^#, (.*)+$)" };
        // captures the "no-" prefix (in case it's in there) so that we know
        // to ignore this entry later
        static const std::wregex printfResourceRegEx{ LR"(\b([a-zA-Z\-])+\b)" };

        std::vector<std::wstring> entryLines;
        std::vector<std::wstring> formatFlags;

        while (!poFileText.empty())
            {
            auto [entryFound, entry] = read_catalog_entry(poFileText);
            if (!entryFound)
                {
                break;
                }
            // step over the section for the next catalog read later
            poFileText.remove_prefix(entry.length());

            entryLines.clear();
            std::copy(std::regex_token_iterator<decltype(entry)::const_iterator>(
                          entry.cbegin(), entry.cend(), entryLineRegEx, 1),
                      std::regex_token_iterator<decltype(entry)::const_iterator>{},
                      std::back_inserter(entryLines));

            po_format_string pofs{ po_format_string::no_format };
            bool formatSpecFound{ false };
            for (const auto& entryLine : entryLines)
                {
                formatFlags.clear();
                std::copy(std::regex_token_iterator<std::wstring::const_iterator>(
                              entryLine.cbegin(), entryLine.cend(), printfResourceRegEx, 0),
                          std::regex_token_iterator<std::wstring::const_iterator>{},
                          std::back_inserter(formatFlags));
                for (const auto& formatFlag : formatFlags)
                    {
                    if (formatFlag == L"c-format" || formatFlag == L"cpp-format")
                        {
                        pofs = po_format_string::cpp_format;
                        formatSpecFound = true;
                        }
                    else if (!is_reviewing_fuzzy_translations() && formatFlag == L"fuzzy")
                        {
                        pofs = po_format_string::no_format;
                        formatSpecFound = true;
                        break;
                        }
                    }
                // stop looking at the comment lines if we loaded a printf specification already
                if (formatSpecFound)
                    {
                    break;
                    }
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
                fileName,
                po_catalog_entry{ std::move(msgId), std::move(msgPluralId),
                                  // when there is a plural, then msgstr 0-1 is where the
                                  // singular and plural translations are kept
                                  msgStr.empty() ? std::move(msg0Str) : std::move(msgStr),
                                  std::move(msg1Str), pofs,
                                  std::vector<std::pair<translation_issue, std::wstring>>{} }));
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

        // find the next blank line, which is the separator between catalog entries
        size_t endOfEntryPos{ 0 };
        while (endOfEntryPos != std::wstring_view::npos)
            {
            endOfEntryPos = poFileText.find(L'\n', endOfEntryPos);
            // we must be at the last entry
            if (endOfEntryPos == std::wstring_view::npos ||
                endOfEntryPos == poFileText.length() - 1)
                {
                return std::make_pair(true, poFileText);
                }
            ++endOfEntryPos;
            // eat up whitespace on line
            while (endOfEntryPos < poFileText.length() - 1 &&
                   string_util::is_either(poFileText[endOfEntryPos], L'\t', L' '))
                {
                ++endOfEntryPos;
                }
            // stop if we encountered a blank line (with or without empty whitespace in it)
            if (endOfEntryPos == poFileText.length() - 1 ||
                string_util::is_either(poFileText[endOfEntryPos], L'\r', L'\n'))
                {
                break;
                }
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
    std::vector<std::wstring>
    po_file_review::convert_positional_cpp_printf(const std::vector<std::wstring>& printfCommands,
                                                  std::wstring& errorInfo)
        {
        errorInfo.clear();

        const std::wregex positionalRegex{ L"^%([[:digit:]]+)[$](.*)" };

        std::map<long, std::wstring> positionalCommands;
        std::vector<std::wstring> adjustedCommands{ printfCommands };

        size_t nonPositionalCommands{ 0 };
        for (const auto& pc : printfCommands)
            {
            std::match_results<std::wstring::const_iterator> matches;
            if (std::regex_search(pc, matches, positionalRegex))
                {
                if (matches.size() >= 3)
                    {
                    // position will need to be zero-indexed
                    long position = std::wcstol(matches[1].str().c_str(), nullptr, 10) - 1;
                    const auto [insertionPos, inserted] = positionalCommands.insert(
                        std::make_pair(position, L"%" + matches[2].str()));
                    // if positional argument is used more than once, make sure they are consistent
                    if (!inserted)
                        {
                        if (insertionPos->second != L"%" + matches[2].str())
                            {
                            errorInfo = L"('" + matches[0].str() +
                                        L"': positional argument provided more than once, but with "
                                        "different data types.)";
                            return std::vector<std::wstring>{};
                            }
                        }
                    };
                }
            else
                {
                ++nonPositionalCommands;
                }
            }

        // Fill output commands from positional arguments.
        // Note that you cannot mix positional and non-positional arguments
        // in the same printf string. If that is happening here, then the
        // non-positional ones will be thrown out and be recorded as an error later.
        if (positionalCommands.size())
            {
            if (nonPositionalCommands > 0)
                {
                errorInfo =
                    L"(Positional and non-positional commands mixed in the same printf string.)";
                }
            adjustedCommands.clear();
            for (auto& posArg : positionalCommands)
                {
                adjustedCommands.push_back(std::move(posArg.second));
                }
            }

        return adjustedCommands;
        }

    //------------------------------------------------
    std::vector<std::wstring> po_file_review::load_cpp_printf_commands(const std::wstring& resource,
                                                                       std::wstring& errorInfo)
        {
        std::vector<std::pair<size_t, std::wstring>> results;

        // we need to do this multipass because a single regex command for all printf
        // commands is too complex and will cause the regex library to randomly throw exceptions
        std::wstring::const_iterator searchStart(resource.cbegin());
        std::wsmatch res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_int_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_float_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_string_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_pointer_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        std::vector<std::wstring> finalStrings;
        finalStrings.reserve(results.size());
        for (auto& res : results)
            {
            finalStrings.push_back(std::move(res.second));
            }

        return convert_positional_cpp_printf(finalStrings, errorInfo);
        }

    //------------------------------------------------
    void po_file_review::review_prinf_issues()
        {
        std::vector<std::wstring> printfStrings1, printfStrings2;
        std::wstring errorInfo;
        for (auto& catEntry : m_catalog_entries)
            {
            if (catEntry.second.m_po_format == po_format_string::cpp_format)
                {
                // only look at strings that have a translation
                if (!catEntry.second.m_translation.empty())
                    {
                    printfStrings1 = load_cpp_printf_commands(catEntry.second.m_source, errorInfo);
                    printfStrings2 =
                        load_cpp_printf_commands(catEntry.second.m_translation, errorInfo);

                    if (printfStrings1.size() || printfStrings2.size())
                        {
                        if (printfStrings1 != printfStrings2)
                            {
                            catEntry.second.m_issues.emplace_back(
                                translation_issue::printf_issue,
                                L"\"" + catEntry.second.m_source + L"\" vs. \"" +
                                    catEntry.second.m_translation + L"\"" + errorInfo);
                            }
                        }
                    }

                if (!catEntry.second.m_translation_plural.empty())
                    {
                    printfStrings1 =
                        load_cpp_printf_commands(catEntry.second.m_source_plural, errorInfo);
                    printfStrings2 =
                        load_cpp_printf_commands(catEntry.second.m_translation_plural, errorInfo);

                    if (printfStrings1.size() || printfStrings2.size())
                        {
                        if (printfStrings1 != printfStrings2)
                            {
                            catEntry.second.m_issues.emplace_back(
                                translation_issue::printf_issue,
                                L"\"" + catEntry.second.m_source_plural + L"\" vs. \"" +
                                    catEntry.second.m_translation_plural + L"\"" + errorInfo);
                            }
                        }
                    }
                }
            }
        }
    } // namespace i18n_check
