///////////////////////////////////////////////////////////////////////////////
// Name:        pseudo_translate.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "pseudo_translate.h"

namespace i18n_check
    {
    const std::map<wchar_t, wchar_t> pseudo_translater::m_euro_char_map = {
        { L'a', L'\u00E0' }, { L'A', L'\u00C0' }, { L'b', L'\u0180' }, { L'B', L'\u0181' },
        { L'c', L'\u00E7' }, { L'C', L'\u00C7' }, { L'd', L'\u010F' }, { L'D', L'\u010E' },
        { L'e', L'\u00EA' }, { L'E', L'\u00CA' }, { L'f', L'\u0192' }, { L'F', L'\u0191' },
        { L'g', L'\u01F5' }, { L'G', L'\u0193' }, { L'h', L'\u1E25' }, { L'H', L'\u1E24' },
        { L'i', L'\u00EC' }, { L'I', L'\u00CC' }, { L'j', L'\u0249' }, { L'J', L'\u0248' },
        { L'k', L'\u01E9' }, { L'K', L'\u01E8' }, { L'l', L'\u0142' }, { L'L', L'\u0141' },
        { L'm', L'\u1E41' }, { L'M', L'\u1E40' }, { L'n', L'\u0148' }, { L'N', L'\u0147' },
        { L'o', L'\u00F6' }, { L'O', L'\u00D6' }, { L'p', L'\u0440' }, { L'P', L'\u0420' },
        { L'q', L'\u024A' }, { L'Q', L'\u024A' }, { L'r', L'\u0213' }, { L'R', L'\u0212' },
        { L's', L'\u015B' }, { L'S', L'\u015A' }, { L't', L'\u021B' }, { L'T', L'\u021A' },
        { L'u', L'\u00FC' }, { L'U', L'\u00DC' }, { L'v', L'\u1E7F' }, { L'V', L'\u1E7E' },
        { L'w', L'\u1E87' }, { L'W', L'\u1E86' }, { L'x', L'\u0445' }, { L'X', L'\u0425' },
        { L'y', L'\u00FD' }, { L'Y', L'\u00DD' }, { L'z', L'\u01B6' }, { L'Z', L'\u01B5' },
        { L'0', L'\u2070' }, { L'1', L'\u2081' }, { L'2', L'\u01BB' }, { L'3', L'\u01B7' },
        { L'4', L'\u2463' }, { L'5', L'\u01BD' }, { L'6', L'\u2465' }, { L'7', L'\u247A' },
        { L'8', L'\u0223' }, { L'9', L'\u277E' }
    };

    //------------------------------------------------
    void pseudo_translater::translate_po_file(std::wstring& poFileText) const
        {
        if (poFileText.empty())
            {
            return;
            }

        static const std::wstring_view MSGID{ L"msgid \"" };
        static const std::wstring_view MSGID_PLURAL{ L"msgid_plural \"" };
        static const std::wstring_view MSGSTR{ L"msgstr \"" };
        static const std::wstring_view MSGSTR0{ L"msgstr[0] \"" };
        static const std::wstring_view MSGSTR1{ L"msgstr[1] \"" };

        std::wstring_view fileContent{ poFileText };

        std::wsmatch res;
        size_t currentPosition{ 0 };

        // find the first blank line so that we can skip over the header section
        while (true)
            {
            const size_t newLinePos = fileContent.find(L'\n', currentPosition);
            // if no blank lines, then bail as there will be nothing to load
            if (newLinePos == std::wstring_view::npos || newLinePos == fileContent.length() - 1)
                {
                return;
                }
            if (fileContent[newLinePos + 1] == L'\r' || fileContent[newLinePos + 1] == L'\n')
                {
                currentPosition = newLinePos;
                fileContent.remove_prefix(currentPosition);
                break;
                }
            currentPosition = newLinePos + 1;
            }

        auto [foundEntry, entryContent, entryPos] = i18n_review::read_po_catalog_entry(fileContent);
        while (foundEntry)
            {
            // step to start of catalog entry
            currentPosition += entryPos;
            fileContent = std::wstring_view{ poFileText }.substr(currentPosition);

            int64_t altertedLenthDiff{ 0 };

            // read the main source string
            std::wstring_view msgIdEntry{ entryContent };
            const auto [foundMsgId, msgIdContent, msgIdPos, msgIdLen] =
                i18n_review::read_po_msg(msgIdEntry, MSGID);
            const auto [foundMsgPluralId, msgIdPluralContent, msgIdPluralPos, msgIdPluralLen] =
                i18n_review::read_po_msg(msgIdEntry, MSGID_PLURAL);
            if (foundMsgId)
                {
                // if there is a plural source string, then pseudo-translate msgstr[0] based
                // on the singular form; otherwise, pseudo-translate msgstr.
                const std::wstring_view msgStrKey = (foundMsgPluralId ? MSGSTR0 : MSGSTR);
                // read the main translation...
                std::wstring_view msgStrEntry{ entryContent };
                const auto [foundMsgStr, msgStrContent, msgStrPos, msgStrLen] =
                    i18n_review::read_po_msg(msgStrEntry, msgStrKey);
                if (foundMsgStr)
                    {
                    // ...and replace it with a pseudo-translation
                    altertedLenthDiff = msgIdLen - msgStrLen;
                    poFileText.replace(currentPosition + msgStrPos + msgStrKey.length(), msgStrLen,
                                       mutate_message(msgIdContent));
                    }
                }
            // if a plural form of the source string exists, then pseudo-translate msgstr[1]
            // based on that
            if (foundMsgPluralId)
                {
                // read the plural translation...
                std::wstring_view msgStrEntry{ entryContent };
                const auto [foundMsgStr, msgStrContent, msgStrPos, msgStrLen] =
                    i18n_review::read_po_msg(msgStrEntry, MSGSTR1);
                if (foundMsgStr)
                    {
                    // ...and replace it with a pseudo-translation
                    altertedLenthDiff += msgIdPluralLen - msgStrLen;
                    poFileText.replace(currentPosition + msgStrPos + MSGSTR1.length(), msgStrLen,
                                       mutate_message(msgIdPluralContent));
                    }
                }

            // step to end of catalog entry and look for next one
            currentPosition += entryContent.length() + altertedLenthDiff;
            fileContent = std::wstring_view{ poFileText }.substr(currentPosition);
            std::tie(foundEntry, entryContent, entryPos) =
                i18n_review::read_po_catalog_entry(fileContent);
            }

        // remove any fuzzy specifiers
        const std::wstring_view FUZZY{ L"#, fuzzy" };
        size_t foundPos = poFileText.find(FUZZY);
        while (foundPos != std::wstring::npos && foundPos > 0)
            {
            size_t lastChar = poFileText.find_last_not_of(L"\r\n", foundPos - 1);
            if (lastChar == std::wstring::npos)
                {
                break;
                }
            // If line above ends in a quote, this it is probably a different entry.
            // That means this entry is missing references and is probably a commented
            // out section. In that case, skip over it.
            if (poFileText[lastChar] == L'"')
                {
                foundPos = poFileText.find(L"#, fuzzy", foundPos + FUZZY.length());
                continue;
                }
            ++lastChar; // step forward to the first newline character
            size_t nextChar = poFileText.find_first_not_of(L"\r\n", foundPos + FUZZY.length());
            if (nextChar == std::wstring::npos)
                {
                break;
                }
            --nextChar; // step back to last newline
            poFileText.replace(lastChar, nextChar - lastChar, L"");
            foundPos = poFileText.find(L"#, fuzzy", foundPos);
            }

        // mark the file's encoding as UTF-8
        const std::wregex CONTENT_TYPE_RE{
            LR"((\r|\n)\"Content-Type:[ ]*text/plain;[ ]*charset[ ]*=[ ]*([a-zA-Z0-9\-]+))"
        };
        std::wsmatch matches;
        if (std::regex_search(poFileText, matches, CONTENT_TYPE_RE) && matches.size() >= 3)
            {
            poFileText.replace(matches.position(2), matches.length(2), L"UTF-8");
            }
        }

    //------------------------------------------------
    std::wstring pseudo_translater::mutate_message(const std::wstring& msg) const
        {
        const auto printfSpecifiers = i18n_review::load_cpp_printf_command_positions(msg);

        std::wstring newMsg;
        newMsg.reserve(m_add_surrounding_brackets ? msg.size() + 6 : msg.size());
        for (size_t i = 0; i < msg.length(); /* handled in loop*/)
            {
            // step over printf commands
            const auto foundPos = std::find_if(printfSpecifiers.cbegin(), printfSpecifiers.cend(),
                                               [i](auto val) noexcept { return val.first == i; });
            if (foundPos != printfSpecifiers.cend())
                {
                newMsg += msg.substr(i, foundPos->second);
                i += foundPos->second;
                continue;
                }

            if (std::iswalnum(msg[i]))
                {
                if (m_transType == pseudo_translation_method::all_caps)
                    {
                    newMsg += std::towupper(msg[i]);
                    }
                else if (m_transType == pseudo_translation_method::european_characters)
                    {
                    const auto charPos = m_euro_char_map.find(msg[i]);
                    if (charPos != m_euro_char_map.cend())
                        {
                        newMsg += charPos->second;
                        }
                    else
                        {
                        newMsg += msg[i];
                        }
                    }
                else
                    {
                    newMsg += msg[i];
                    }
                }
            else
                {
                newMsg += msg[i];
                }
            ++i;
            }

        if (m_width_increase > 0)
            {
            size_t newCharCountToAdd = static_cast<size_t>(
                std::ceil(msg.length() * (static_cast<double>(m_width_increase) / 100)));
            if (m_add_surrounding_brackets && newCharCountToAdd >= 2)
                {
                newCharCountToAdd -= 2;
                }
            newMsg.insert(0, newCharCountToAdd / 2, L'-');
            newMsg.append(newCharCountToAdd / 2, L'-');
            }

        if (m_add_surrounding_brackets)
            {
            newMsg.insert(0, L"[");
            newMsg.append(L"]");
            }

        return newMsg;
        }

    //------------------------------------------------
    size_t pseudo_translater::find_po_msg_end(const std::wstring& poFileText, const size_t startPos)
        {
        size_t idEndPos{ startPos };
        while (true)
            {
            idEndPos = poFileText.find(L'\"', idEndPos);
            if (idEndPos == std::wstring_view::npos)
                {
                return std::wstring_view::npos;
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
        return idEndPos;
        }
    } // namespace i18n_check
