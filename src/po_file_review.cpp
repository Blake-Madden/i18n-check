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
    void po_file_review::operator()(std::wstring_view poFileText,
                                    const std::filesystem::path& fileName)
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

        // find the first blank line so that we can skip over the header section
        while (true)
            {
            const size_t newLinePos = poFileText.find(L'\n', currentPos);
            // if no blank lines, then bail as there will be nothing to load
            if (newLinePos == std::wstring_view::npos || newLinePos == poFileText.length() - 1)
                {
                return;
                }
            if (poFileText[newLinePos + 1] == L'\r' || poFileText[newLinePos + 1] == L'\n')
                {
                currentPos = newLinePos;
                poFileText.remove_prefix(currentPos);
                break;
                }
            currentPos = newLinePos + 1;
            }

        while (!poFileText.empty())
            {
            auto [entryFound, entry, entryPos] = read_po_catalog_entry(poFileText);
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
                    else if (!is_reviewing_fuzzy_translations() && formatFlag == _DT(L"fuzzy"))
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
            auto [msgIdFound, msgId, msgPos, msgLen] = read_po_msg(entry, MSGID);
            if (!msgIdFound)
                {
                continue;
                }
            // plural is optional, translations may be one string or two (for plural)
            auto [msgIdPluralFound, msgPluralId, msgPosPlural, msgPluralLen] =
                read_po_msg(entry, MSGID_PLURAL);
            auto [msgStrFound, msgStr, msgStrPos, msgStrLen] = read_po_msg(entry, MSGSTR);
            auto [msgStr0Found, msg0Str, msgStr0Pos, msgStr0Len] = read_po_msg(entry, MSGSTR0);
            auto [msgStr1Found, msg1Str, msgStr1Pos, msgStr1Len] = read_po_msg(entry, MSGSTR1);

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
    } // namespace i18n_check
