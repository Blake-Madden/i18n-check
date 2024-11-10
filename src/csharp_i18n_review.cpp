/********************************************************************************
 * Copyright (c) 2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#include "csharp_i18n_review.h"

namespace i18n_check
    {
    //--------------------------------------------------
    void csharp_i18n_review::remove_decorations(std::wstring& str) const
        {
        // get rid of any leading accessors (e.g., '>' (from "->") or '.').
        if (str.length() > 0 && (str.front() == L':' || str.front() == L'>' || str.front() == L'.'))
            {
            const auto colonEnd = str.find_first_not_of(L":>.");
            str.erase(0, (colonEnd == std::wstring::npos) ? str.length() : colonEnd);
            }
        // lop off name of variable from member call
        const auto accessor = str.find_first_of(L">");
        if (accessor != std::wstring::npos)
            {
            str.erase(0, accessor + 1);
            }
        }

    //--------------------------------------------------
    wchar_t* csharp_i18n_review::find_raw_string_end(wchar_t* text,
                                                     [[maybe_unused]] const wchar_t chr) const
        {
        while (*text != 0)
            {
            if (*text != '\"')
                {
                std::advance(text, 1);
                }
            else
                {
                std::advance(text, 1);
                // next character after the stepped-over quote is not a quote or we are at the
                // end of the stream, so return
                if (*text == 0 || *text != '\"')
                    {
                    return std::prev(text);
                    }
                // step over the second quote
                std::advance(text, 1);
                // if on a third quote (or null terminator), then this is the end
                if (*text == 0 || *text == '\"')
                    {
                    return std::prev(text, 2);
                    }
                }
            };
        return text;
        }
    } // namespace i18n_check
