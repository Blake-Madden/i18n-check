///////////////////////////////////////////////////////////////////////////////
// Name:        i18n_string_util.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "i18n_string_util.h"

namespace i18n_string_util
    {
    //--------------------------------------------------
    bool is_url(const wchar_t* text, size_t length)
        {
        if (text == nullptr || length < 5) // NOLINT
            {
            return false;
            }
        // protocols
        if (string_util::strnicmp(text, std::wstring_view{ L"http:" }) == 0)
            {
            return true;
            }
        if (string_util::strnicmp(text, std::wstring_view{ L"https:" }) == 0)
            {
            return true;
            }
        if (string_util::strnicmp(text, std::wstring_view{ L"ftp:" }) == 0)
            {
            return true;
            }
        if (string_util::strnicmp(text, std::wstring_view{ L"www." }) == 0)
            {
            return true;
            }
        if (string_util::strnicmp(text, std::wstring_view{ L"mailto:" }) == 0)
            {
            return true;
            }
        if (string_util::strnicmp(text, std::wstring_view{ L"file:" }) == 0)
            {
            return true;
            }
        // relic from the '90s
        if (string_util::strnicmp(text, std::wstring_view{ L"gopher:" }) == 0)
            {
            return true;
            }

        // an URL that is missing the "www" prefix (e.g, ibm.com/index.html)
        const wchar_t* firstSlash = string_util::strnchr(text, L'/', length);
        if (firstSlash != nullptr)
            {
            const auto lastDotPos = string_util::find_last_of(text, L'.', firstSlash - text);
            if (lastDotPos != std::wstring::npos &&
                (lastDotPos + 4 == static_cast<size_t>(firstSlash - text)) &&
                static_cast<bool>(std::iswalpha(text[lastDotPos + 1])) &&
                static_cast<bool>(std::iswalpha(text[lastDotPos + 2])) &&
                static_cast<bool>(std::iswalpha(text[lastDotPos + 3])))
                {
                return true;
                }
            }

        // cut off possessive form
        if (length >= 3 && is_apostrophe(text[length - 2]) &&
            string_util::is_either(text[length - 1], L's', L'S'))
            {
            length -= 2;
            }

        static const std::set<std::wstring> knownWebExtensions = { L"au",  L"biz", L"ca",
                                                                   L"com", L"edu", L"gov",
                                                                   L"ly",  L"org", L"uk" };

        auto periodPos = string_util::find_last_of(text, L'.', length - 1);
        if (periodPos != std::wstring::npos && periodPos < length - 1)
            {
            ++periodPos;
            if (knownWebExtensions.find(std::wstring(text + periodPos, length - periodPos)) !=
                knownWebExtensions.cend())
                {
                return true;
                }
            }

        return false;
        }

    //--------------------------------------------------
    bool is_file_address(std::wstring_view text)
        {
        if (text.length() < 5)
            {
            return false;
            }
        // protocols
        if (is_url(text.data(), text.length()))
            {
            return true;
            }
        // UNC path
        if (text.length() >= 3 && text[0] == L'\\' && text[1] == L'\\')
            {
            return true;
            }
        // Windows file path
        if (text.length() >= 3 && static_cast<bool>(std::iswalpha(text[0])) && text[1] == L':' &&
            (text[2] == L'\\' || text[2] == L'/'))
            {
            return true;
            }
        // UNIX paths (including where the '/' at the front is missing
        if (text.length() >= 3 && text[0] == L'/' &&
            string_util::strnchr(text.data() + 2, L'/', text.length() - 2))
            {
            return true;
            }
        if (string_util::strnchr(text.data(), L'/', text.length()) != nullptr &&
            (text.starts_with(L"usr/") || text.starts_with(L"var/") || text.starts_with(L"tmp/") ||
             text.starts_with(L"sys/") || text.starts_with(L"srv/") || text.starts_with(L"mnt/") ||
             text.starts_with(L"etc/") || text.starts_with(L"dev/") || text.starts_with(L"bin/") ||
             text.starts_with(L"usr/") || text.starts_with(L"sbin/") ||
             text.starts_with(L"root/") || text.starts_with(L"proc/") ||
             text.starts_with(L"boot/") || text.starts_with(L"home/")))
            {
            return true;
            }

        // email address
        if (text.length() >= 5)
            {
            const wchar_t* spaceInStr =
                string_util::strnchr(text.data() + 1, L' ', text.length() - 1);
            const wchar_t* atSign = string_util::strnchr(text.data() + 1, L'@', text.length() - 1);
            // no spaces and an '@' symbol
            if ((atSign != nullptr) && (spaceInStr == nullptr))
                {
                const wchar_t* dotSign =
                    string_util::strnchr(atSign, L'.', text.length() - (atSign - text.data()));
                if (dotSign && static_cast<size_t>(dotSign - text.data()) < text.length() - 1)
                    {
                    return true;
                    }
                }
            }

        // If a longer string that did not start with a UNIX / or Windows drive letter
        // then this is likely not a file name. It could be filename, but even if it
        // ends with a valid file extension, it would more than likely be a filename
        // at the end of legit sentence if it's this long.
        constexpr size_t maxFileLength{ 128 };
        if (text.length() > maxFileLength)
            {
            return false;
            }

        // cut off possessive form
        if (text.length() >= 3 && is_apostrophe(text[text.length() - 2]) &&
            string_util::is_either(text[text.length() - 1], L's', L'S'))
            {
            text.remove_suffix(2);
            }

        // look at extensions now
        // 3-letter file name
        if (text.length() >= 4 && text[text.length() - 4] == L'.' &&
            static_cast<bool>(std::iswalpha(text[text.length() - 3])) &&
            static_cast<bool>(std::iswalpha(text[text.length() - 2])) &&
            static_cast<bool>(std::iswalpha(text[text.length() - 1])))
            {
            // see if it is really a typo (missing space after a sentence).
            if (std::iswupper(text[text.length() - 3]) && !std::iswupper(text[text.length() - 2]))
                {
                return false;
                }
            // see if a file filter/wildcard (e.g., "*.txt", "Rich Text Format (*.rtf)|*.rtf")
            // and not a file path
            if (text.length() >= 5 && text[text.length() - 5] == L'*')
                {
                return false;
                }
            return true;
            }
        // 4-letter (Microsoft XML-based) file name
        else if (text.length() >= 5 && text[text.length() - 5] == L'.' &&
                 static_cast<bool>(std::iswalpha(text[text.length() - 4])) &&
                 static_cast<bool>(std::iswalpha(text[text.length() - 3])) &&
                 static_cast<bool>(std::iswalpha(text[text.length() - 2])) &&
                 string_util::is_either(text[text.length() - 1], L'x', L'X'))
            {
            // see if it is really a typo (missing space after a sentence)
            if (std::iswupper(text[text.length() - 4]) && !std::iswupper(text[text.length() - 3]))
                {
                return false;
                }
            if (text.length() >= 6 && text[text.length() - 6] == L'*')
                {
                return false;
                }
            return true;
            }
        // 4-letter extensions (HTML)
        else if (text.length() >= 5 && text[text.length() - 5] == L'.' &&
                 string_util::strnicmp(text.data() + (text.length() - 4),
                                       std::wstring_view{ L"html" }) == 0)
            {
            if (text.length() >= 6 && text[text.length() - 6] == L'*')
                {
                return false;
                }
            return true;
            }
        // 2-letter extensions
        else if (text.length() >= 3 && text[text.length() - 3] == L'.' &&
                 // translation, source, and doc files
                 (string_util::strnicmp(text.data() + (text.length() - 2),
                                        std::wstring_view{ L"mo" }) == 0 ||
                  string_util::strnicmp(text.data() + (text.length() - 2),
                                        std::wstring_view{ L"po" }) == 0 ||
                  string_util::strnicmp(text.data() + (text.length() - 2),
                                        std::wstring_view{ L"cs" }) == 0 ||
                  string_util::strnicmp(text.data() + (text.length() - 2),
                                        std::wstring_view{ L"js" }) == 0 ||
                  string_util::strnicmp(text.data() + (text.length() - 2),
                                        std::wstring_view{ L"db" }) == 0 ||
                  string_util::strnicmp(text.data() + (text.length() - 2),
                                        std::wstring_view{ L"md" }) == 0))
            {
            return true;
            }
        // tarball file name
        else if (text.length() >= 7 && string_util::strnicmp(text.data() + (text.length() - 7),
                                                             std::wstring_view{ L".tar." }) == 0)
            {
            // see if it is really a typo (missing space after a sentence).
            if (std::iswupper(text[text.length() - 4]) && !std::iswupper(text[text.length() - 3]))
                {
                return false;
                }
            return true;
            }
        // C header/source files, which only have a letter in the extension,
        // but are common in documentation
        else if (text.length() >= 3 && text[text.length() - 2] == L'.' &&
                 string_util::is_either(text[text.length() - 1], L'h', L'c'))
            {
            return true;
            }

        return false;
        }

    //--------------------------------------------------
    void remove_escaped_unicode_values(std::wstring& str)
        {
        for (size_t i = 0; i < str.length(); /* in loop*/)
            {
            // '\' that is not escaped by a proceeding '\'
            if (str[i] == L'\\' && (i == 0 || str[(i - 1)] != L'\\'))
                {
                // "\u266F" format
                if (i + 5 < str.length() && str[i + 1] == L'u' &&
                    string_util::is_hex_digit(str[i + 2]) &&
                    string_util::is_hex_digit(str[i + 3]) &&
                    string_util::is_hex_digit(str[i + 4]) && string_util::is_hex_digit(str[i + 5]))
                    {
                    str.replace(i, 6, 6, ' ');
                    i += 6;
                    continue;
                    }
                // "\U000FF254" format
                if (i + 8 < str.length() && str[i + 1] == L'U' &&
                         string_util::is_hex_digit(str[i + 2]) &&
                         string_util::is_hex_digit(str[i + 3]) &&
                         string_util::is_hex_digit(str[i + 4]) &&
                         string_util::is_hex_digit(str[i + 5]) &&
                         string_util::is_hex_digit(str[i + 6]) &&
                         string_util::is_hex_digit(str[i + 7]) &&
                         string_util::is_hex_digit(str[i + 8]))
                    {
                    str.replace(i, 10, 10, ' ');
                    i += 10;
                    continue;
                    }
                // "\xFFFF" format (can be variable number of hex digits)
                if (i + 5 < str.length() && str[i + 1] == L'x' &&
                         // at least two hex digits needed
                         string_util::is_hex_digit(str[i + 2]) &&
                         string_util::is_hex_digit(str[i + 3]) &&
                         string_util::is_hex_digit(str[i + 4]) &&
                         string_util::is_hex_digit(str[i + 5]))
                    {
                    str.replace(i, 6, 6, ' ');
                    i += 6;
                    continue;
                    }
                // "\xFF" format (can be variable number of hex digits)
                if (i + 3 < str.length() && str[i + 1] == L'x' &&
                         // at least two hex digits needed
                         string_util::is_hex_digit(str[i + 2]) &&
                         string_util::is_hex_digit(str[i + 3]))
                    {
                    str.replace(i, 4, 4, ' ');
                    i += 4;
                    continue;
                    }
                }
            ++i;
            }
        }
    } // namespace i18n_string_util
