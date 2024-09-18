///////////////////////////////////////////////////////////////////////////////
// Name:        po_file_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "po_file_review.h"

namespace i18n_check
    {
    //------------------------------------------------
    void po_file_review::operator()(std::wstring_view poFileText, const std::wstring& fileName)
        {
        m_file_name = fileName;

        const std::wstring_view originalPoFileText{ poFileText };

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
        static const std::wregex entryLineRegEx{ LR"(^#, ([,a-z \-]+)+$)",
        // MSVC doesn't have the std::regex::multiline, but behaves multiline implicitly.
        // GCC and Clang require this flag though.
#ifdef _MSC_VER
                                                 std::regex::ECMAScript };
#else
                                                 std::regex::ECMAScript | std::regex::multiline };
#endif

        // captures the "no-" prefix (in case it's in there) so that we know
        // to ignore this entry later
        static const std::wregex printfResourceRegEx{ LR"(\b([a-zA-Z\-])+\b)" };

        std::vector<std::wstring> entryLines;
        std::vector<std::wstring> formatFlags;

        size_t currentPos{ 0 };

        while (!poFileText.empty())
            {
            auto [entryFound, entry, entryPos] = read_catalog_entry(poFileText);
            if (!entryFound)
                {
                break;
                }
            // step over the section for the next catalog read later
            poFileText.remove_prefix(entry.length());
            // update the position in the original text of where this entry is
            currentPos += entryPos + entry.length();

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

            get_catalog_entries().push_back(std::make_pair(
                fileName,
                translation_catalog_entry{
                    std::move(msgId), std::move(msgPluralId),
                    // when there is a plural, then msgstr 0-1 is where the
                    // singular and plural translations are kept
                    msgStr.empty() ? std::move(msg0Str) : std::move(msgStr), std::move(msg1Str),
                    pofs, std::vector<std::pair<translation_issue, std::wstring>>{},
                    get_line_and_column(currentPos, originalPoFileText).first }));
            }
        }

    //------------------------------------------------
    std::tuple<bool, std::wstring_view, size_t>
    po_file_review::read_catalog_entry(std::wstring_view& poFileText)
        {
        const size_t entryPos = poFileText.find(L"\n#");
        if (entryPos == std::wstring_view::npos)
            {
            return { false, std::wstring_view{}, std::wstring_view::npos };
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
                return { true, poFileText, entryPos };
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
        return { true, poFileText.substr(0, endOfEntryPos), entryPos };
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
        // Step back to see if this is a previous translation (#|) or commented
        // out translation (#~).
        size_t lookBehindIndex{ idPos };
        while (lookBehindIndex > 0 &&
               string_util::is_neither(poFileText[lookBehindIndex], L'\r', L'\n'))
            {
            --lookBehindIndex;
            }
        if (poFileText[++lookBehindIndex] == L'#')
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
                // jump to next line
                while (lookAheadIndex < poFileText.length() &&
                       string_util::is_either(poFileText[lookAheadIndex], L'\r', L'\n'))
                    {
                    ++lookAheadIndex;
                    }
                // eat up leading spaces
                while (lookAheadIndex < poFileText.length() &&
                       string_util::is_either(poFileText[lookAheadIndex], L'\t', L' '))
                    {
                    ++lookAheadIndex;
                    }
                // if a quote, then this is still be part of the same string
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
    } // namespace i18n_check
