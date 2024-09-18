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
    //------------------------------------------------
    void pseudo_translater::po_file(std::wstring& poFileText) const
        {
        if (poFileText.empty())
            {
            return;
            }

        static const std::wregex msgstr_regex{ LR"((\r|\n)+msgstr(\[[0-9]+\])? ")" };
        static const std::wstring_view MSGID{ L"msgid \"" };
        static const std::wstring_view MSGID_PLURAL{ L"msgid_plural \"" };
        static const std::wstring_view MSGSTR{ L"msgstr \"" };
        static const std::wstring_view MSGSTR0{ L"msgstr[0] \"" };
        static const std::wstring_view MSGSTR1{ L"msgstr[1] \"" };

        std::wsmatch res;
        size_t currentPosition{ 0 };

        std::wstring_view fileContent{ poFileText };

        auto [foundEntry, entryContent, entryPos] = i18n_review::read_po_catalog_entry(fileContent);
        while (foundEntry)
            {
            // step to start of catalog entry
            currentPosition += entryPos;
            fileContent = std::wstring_view{ poFileText }.substr(currentPosition);

            std::wstring_view msgIdEntry{ entryContent };
            const auto [foundMsgId, msgIdContent, msgIdPos, msgIdLen] =
                i18n_review::read_po_msg(msgIdEntry, MSGID);
            int64_t altertedLenthDiff{ 0 };
            if (foundMsgId)
                {
                std::wstring_view msgStrEntry{ entryContent };
                const auto [foundMsgStr, msgStrContent, msgStrPos, msgStrLen] =
                    i18n_review::read_po_msg(msgStrEntry, MSGSTR);
                if (foundMsgStr)
                    {
                    altertedLenthDiff = msgIdLen - msgStrLen;
                    if (m_transType == pseudo_translation_method::all_caps)
                        {
                        poFileText.replace(currentPosition + msgStrPos + MSGSTR.length(), msgStrLen,
                                           mutate_message_caps(msgIdContent));
                        }
                    }
                }

            // step to end of catalog entry and look for next one
            currentPosition += entryContent.length();
            fileContent =
                std::wstring_view{ poFileText }.substr(currentPosition + altertedLenthDiff);
            std::tie(foundEntry, entryContent, entryPos) =
                i18n_review::read_po_catalog_entry(fileContent);
            }
        }

    //------------------------------------------------
    std::wstring pseudo_translater::mutate_message_caps(const std::wstring& msg)
        {
        const auto printfSpecifiers = i18n_review::load_cpp_printf_command_positions(msg);

        std::wstring newMsg;
        newMsg.reserve(msg.size());
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

            if (std::iswalpha(msg[i]))
                {
                newMsg += std::towupper(msg[i]);
                }
            else
                {
                newMsg += msg[i];
                }
            ++i;
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
