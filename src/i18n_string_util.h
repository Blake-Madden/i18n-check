/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2023
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_EXTRACT_H__
#define __I18N_EXTRACT_H__

#include "char_traits.h"
#include "string_util.h"
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <regex>
#include <set>
#include <string>
#include <string_view>

/// @brief Helper functions for reviewing i18n/l10n related strings.
namespace i18n_string_util
    {
    /** @brief Determines if a string is a local file path,
            file name, email address, or internet address.
        @returns @c true if text block is a local file or Internet address.
        @param text The text block to analyze.
        @param length The length of the text block to analyze.\n
            This will be the start of the text block up to the end of the
            suspected file address.*/
    [[nodiscard]]
    bool is_file_address(const wchar_t* text, size_t length);

    /** @brief Determines if a string is an internet address.
        @returns @c true if text block is an Internet address.
        @param text The text block to analyze.
        @param length The length of the text block to analyze.\n
            This will be the start of the text block up to the end of the
            suspected file address.*/
    [[nodiscard]]
    bool is_url(const wchar_t* text, size_t length);

    /** @returns Whether a character is a number (narrow [0-9] characters only).
        @param ch The letter to be reviewed.*/
    [[nodiscard]]
    constexpr static bool is_numeric(const wchar_t ch) noexcept
        {
        return (ch >= L'0' && ch <= L'9') ? true : false;
        }

    /** @returns @c true if a character is a letter
            (English alphabet only, and no full-width characters).
        @param ch The letter to be reviewed.*/
    [[nodiscard]]
    constexpr static bool is_alpha_7bit(const wchar_t ch) noexcept
        {
        return (((ch >= 0x41 /*'A'*/) && (ch <= 0x5A /*'Z'*/)) ||
                ((ch >= 0x61 /*'a'*/) && (ch <= 0x7A /*'z'*/)));
        }

    /** @returns @c true if a character is an apostrophe (includes straight single quotes).
        @param ch The letter to be reviewed.*/
    [[nodiscard]]
    constexpr static bool is_apostrophe(const wchar_t ch) noexcept
        {
        // clang-format off
        return (ch == 39) ?         // '
            true : (ch == 146) ?    // apostrophe
            true : (ch == 180) ?    // apostrophe
            true : (ch == 0xFF07) ? // full-width apostrophe
            true : (ch == 0x2019) ? // right single apostrophe
            true : false;
        // clang-format on
        }

    /// @brief Removes printf commands in @c str (in-place).
    /// @param str The string to have printf commands removed from.
    inline void remove_printf_commands(std::wstring& str)
        {
        // Y H M are also included, as they are for similar datetime formatting functions
        static const std::wregex printfRegex(
            L"([^%\\\\]|^|\\b)%[-+0 #]{0,4}[.[:digit:]]*"
            "(?:c|C|d|i|o|u|lu|ld|lx|lX|lo|llu|lld|x|X|e|E|f|g|G|a|A|n|p|s|S|Z|zu|Y|H|M)");
        // The % command (not following another % or \),
        // flags ("-+0 #", optionally can have up to 4 of these),
        // width and precision (".0-9", optional), and the specifier.
        try
            {
            str = std::regex_replace(str, printfRegex, L"$1");
            return;
            }
        catch (...)
            {
            return;
            }
        }

    /// @brief Removes hex color values (e.g., "#FF00AA") in @c str (in-place).
    /// @param str The string to have color values removed from.
    inline void remove_hex_color_values(std::wstring& str)
        {
        const std::wregex colorRegex(L"#[[:xdigit:]]{6}");
        try
            {
            str = std::regex_replace(str, colorRegex, L"");
            return;
            }
        catch (...)
            {
            return;
            }
        }

    /// @brief Removes escaped unicode values in @c str.
    ///     (e.g., "\u266f" will be replaced with spaces).
    /// @param[out] str The string being escaped.
    void remove_escaped_unicode_values(std::wstring& str);

    /** @brief Converts a string to wstring (assuming that the string is simple 8-bit ASCII).
        @param str The string to convert.
        @returns The string, converted to a wstring.
        @warning This assumes 8-bit ASCII strings and does not perform any sort
            of charset conversion. This should only be used for very simple strings,
            such as `what()` from a untranslated `std::exception`.*/
    [[nodiscard]]
    inline std::wstring lazy_string_to_wstring(const std::string& str)
        {
        std::wstring retVal;
        retVal.reserve(str.length());
        for (const auto& ch : str)
            {
            retVal += static_cast<wchar_t>(ch);
            }
        return retVal;
        }

    /// @brief Converts escaped control characters (e.g., "\n")
    ///     inside of a string into spaces.
    /// @param[out] str The string being escaped.
    template<typename string_typeT>
    void replace_escaped_control_chars(string_typeT& str)
        {
        for (size_t i = 0; i < str.length(); ++i)
            {
            if (str[i] == L'\\' &&
                (str[i + 1] == L'n' || str[i + 1] == L'r' || str[i + 1] == L't') &&
                (i == 0 || str[i - 1] != L'\\'))
                {
                str[i] = str[i + 1] = L' ';
                }
            }
        }
    } // namespace i18n_string_util

/** @}*/

#endif //__I18N_EXTRACT_H__
