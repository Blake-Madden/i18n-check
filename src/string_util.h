/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2004-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
    it under the terms of the BSD License.
* @{*/

#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <cwctype>
#include <cctype>
#include <cstring>
#include <cwchar>
#include <cstddef>
#include <string>
#include <functional>
#include <cassert>
#include <clocale>
#include <stdexcept>
#include <regex>

namespace string_util
    {
    ///ANSI C decorators
    ///strtol
    inline double strtol(const char* str, char** strend, int radix) noexcept
        { return std::strtol(str, strend, radix); }
    inline double strtol(const wchar_t* str, wchar_t** strend, int radix) noexcept
        { return std::wcstol(str, strend, radix); }
    ///strtod
    inline double strtod(const char* str, char** strend) noexcept
        { return std::strtod(str, strend); }
    inline double strtod(const wchar_t* str, wchar_t** strend) noexcept
        { return std::wcstod(str, strend); }
    ///atoi
    inline int atoi(const char* str) noexcept
        {
        if (str == nullptr)
            { return 0; }
        return std::atoi(str);
        }
    inline int atoi(const wchar_t* str) noexcept
        {
        if (str == nullptr)
            { return 0; }
        wchar_t* dummy = nullptr;
        return static_cast<int>(std::wcstol(str, &dummy, 10));
        }
    ///atol
    inline long atol(const char* str) noexcept
        {
        if (str == nullptr)
            { return 0; }
        return std::atol(str);
        }
    inline long atol(const wchar_t* str) noexcept
        {
        if (str == nullptr)
            { return 0; }
        wchar_t* dummy = nullptr;
        return std::wcstol(str, &dummy, 10);
        }
    ///tolower
    inline int tolower(char c) noexcept
        {
        return std::tolower(static_cast<unsigned char>(c));
        }
    inline wchar_t tolower(wchar_t c) noexcept
        {
        return std::towlower(c);
        }
    ///toupper
    inline int toupper(char c) noexcept
        {
        return std::toupper(static_cast<unsigned char>(c));
        }
    inline wchar_t toupper(wchar_t c) noexcept
        {
        return std::towupper(c);
        }
    ///memset
    template<typename T>
    inline T* memset(T* dest, int c, size_t count)
        {
        return static_cast<T*>(std::memset(dest, c, count));
        }
    //partial specialized versions of memset
    inline char* memset(char* dest, int c, size_t count) noexcept
        {
        return static_cast<char*>(std::memset(dest, c, count));
        }
    inline wchar_t* memset(wchar_t* dest, int c, size_t count) noexcept
        {
        return std::wmemset(dest, static_cast<wchar_t>(c), count);
        }
    ///strchr
    inline const char* strchr(const char* s, int ch) noexcept
        {
        return std::strchr(s, ch);
        }
    inline const wchar_t* strchr(const wchar_t* s, wchar_t ch) noexcept
        {
        return std::wcschr(s, ch);
        }
    ///strstr
    inline const char* strstr(const char* s1, const char* s2) noexcept
        {
        return std::strstr(s1, s2);
        }
    inline const wchar_t* strstr(const wchar_t* s1, const wchar_t* s2) noexcept
        {
        return std::wcsstr(s1, s2);
        }
    ///strcspn
    inline size_t strcspn(const char* string1,const char* string2) noexcept
        {
        return std::strcspn(string1, string2);
        }
    inline size_t strcspn(const wchar_t* string1,const wchar_t* string2) noexcept
        {
        return std::wcscspn(string1, string2);
        }
    ///strncat
    inline char* strncat(char* strDest, const char* strSource, size_t count) noexcept
        {
        return std::strncat(strDest, strSource, count);
        }
    inline wchar_t* strncat(wchar_t* strDest, const wchar_t* strSource, size_t count) noexcept
        {
        return std::wcsncat(strDest, strSource, count);
        }
    ///wctomb
    inline int wctomb(wchar_t* s, wchar_t wc) noexcept
        {
        assert(s);
        if (s)
            { s[0] = wc; }
        return -1;
        }
    inline int wctomb(char* s, wchar_t wc) noexcept
        {
        return std::wctomb(s, wc);
        }
    //strlen
    inline size_t strlen(const char* text) noexcept
        { return std::strlen(text); }
    inline size_t strlen(const wchar_t* text) noexcept
        { return std::wcslen(text); }
    ///strcmp
    inline int strcmp(const char* string1, const char* string2) noexcept
        {
        return std::strcmp(string1, string2);
        }
    inline int strcmp(const wchar_t* string1, const wchar_t* string2) noexcept
        {
        return std::wcscmp(string1, string2);
        }
    ///strncmp
    inline int strncmp(const char* string1, const char* string2, size_t count) noexcept
        {
        return std::strncmp(string1, string2, count);
        }
    inline int strncmp(const wchar_t* string1, const wchar_t* string2, size_t count) noexcept
        {
        return std::wcsncmp(string1, string2, count);
        }
    ///strncpy
    inline char* strncpy(char* strDest, const char* strSource, size_t count) noexcept
        {
        return std::strncpy(strDest, strSource, count);
        }
    inline wchar_t* strncpy(wchar_t* strDest, const wchar_t* strSource, size_t count) noexcept
        {
        return std::wcsncpy(strDest, strSource, count);
        }

    /// Determines if a given value is either of two other given values.
    /// @param value The value to compare with.
    /// @param first The first value to compare against.
    /// @param second The second value to compare against.
    /// @returns True if value is either of the other values.
    template<typename T>
    [[nodiscard]] constexpr bool is_either(const T value, const T first, const T second) noexcept
        { return (value == first || value == second); }

    /// Determines if a given value is neither of two other given values.
    /// @param value The value to compare with.
    /// @param first The first value to compare against.
    /// @param second The second value to compare against.
    /// @returns True if value is neither of the other values.
    template<typename T>
    [[nodiscard]] constexpr bool is_neither(const T value, const T first, const T second) noexcept
        {
        assert(first != second);
        return (value != first && value != second);
        }

    /** @returns Whether a character is a number (0-9 characters only, narrow versions).
        @param ch The letter to be reviewed.*/
    [[nodiscard]] static constexpr bool is_numeric_8bit(const wchar_t ch) noexcept
        {
        return (ch >= L'0' && ch <= L'9') ?
            true : false;
        }

    /// @returns True if @c ch is a subscript number.
    /// @param ch The chacter to review.
    [[nodiscard]] static inline constexpr bool is_subscript_number(const wchar_t ch) noexcept
        { return (ch >= 0x2080 && ch <= 0x2089); }

    /// @returns True if @c ch is a subscript.
    /// @param ch The chacter to review.
    [[nodiscard]] static inline constexpr bool is_subscript(const wchar_t ch) noexcept
        {
        return (ch >= 0x2080 && ch <= 0x2089) ||
            (ch == 0x208A) ||
            (ch == 0x208B) ||
            (ch == 0x208C) ||
            (ch == 0x208D) ||
            (ch == 0x208E) ||
            (ch == 0x2090) ||
            (ch == 0x2091) ||
            (ch == 0x2092) ||
            (ch == 0x2093) ||
            (ch == 0x2094) ||
            (ch == 0x2095) ||
            (ch == 0x2096) ||
            (ch == 0x2097) ||
            (ch == 0x2098) ||
            (ch == 0x2099) ||
            (ch == 0x209A) ||
            (ch == 0x209B) ||
            (ch == 0x209C);
        }

    /** Converts a character into its subscript equivalent.
        @param ch The character to convert.
        @returns The character converted into its subscript equivalent, or
                 the original value if it can't be converted.
        @note This only applies to numbers, simple math characters, and a few letters (e.g., 2 -> ²)*/
    [[nodiscard]] static inline constexpr wchar_t to_subscript(const wchar_t ch) noexcept
        {
        return (ch == L'0' || ch == 0xFF10) ? 0x2080 :
            (ch == L'1' || ch == 0xFF11) ? 0x2081 :
            (ch == L'2' || ch == 0xFF12) ? 0x2082 :
            (ch == L'3' || ch == 0xFF13) ? 0x2083 :
            (ch == L'4' || ch == 0xFF14) ? 0x2084 :
            (ch == L'5' || ch == 0xFF15) ? 0x2085 :
            (ch == L'6' || ch == 0xFF16) ? 0x2086 :
            (ch == L'7' || ch == 0xFF17) ? 0x2087 :
            (ch == L'8' || ch == 0xFF18) ? 0x2088 :
            (ch == L'9' || ch == 0xFF19) ? 0x2089 :
            // simple math characters and letters
            (ch == L'+') ? 0x208A :
            (ch == L'-') ? 0x208B :
            (ch == L'=') ? 0x208C :
            (ch == L'(') ? 0x208D :
            (ch == L')') ? 0x208E :
            (ch == L'a') ? 0x2090 :
            (ch == L'e') ? 0x2091 :
            (ch == L'o') ? 0x2092 :
            (ch == L'x') ? 0x2093 :
            // 0x2094 is upsidedown 'e', no real equivalent with this
            (ch == L'h') ? 0x2095 :
            (ch == L'k') ? 0x2096 :
            (ch == L'l') ? 0x2097 :
            (ch == L'm') ? 0x2098 :
            (ch == L'n') ? 0x2099 :
            (ch == L'p') ? 0x209A :
            (ch == L's') ? 0x209B :
            (ch == L't') ? 0x209C :
            ch;
        }

    /// @returns True if @c ch is a fraction character.
    /// @param ch The chacter to review.
    [[nodiscard]] static inline constexpr bool is_fraction(const wchar_t ch) noexcept
        {
        return (ch == 0xBC || ch == 0xBD || ch == 0xBE);
        }

    /// @returns True if @c ch is a superscript number.
    /// @param ch The chacter to review.
    /// @note This includes lowercased Roman numeral letters.
    [[nodiscard]] static inline constexpr bool is_superscript_number(const wchar_t ch) noexcept
        {
        return (ch == 0x2070) ||
            (ch == 0xB9) ||
            (ch == 0xB2) ||
            (ch == 0xB3) ||
            (ch == 0x2074) ||
            (ch == 0x2075) ||
            (ch == 0x2076) ||
            (ch == 0x2077) ||
            (ch == 0x2078) ||
            (ch == 0x2079) ||
            // Roman numerals
            (ch == 0x1D9C) || // c
            (ch == 0x1D48) || // d
            (ch == 0x2071) || // i
            (ch == 0x1D50) || // m
            (ch == 0x1D5B) || // v
            (ch == 0x02E3);   // x
        }

    /// @returns True if @c ch is a lowercased superscript letter.
    /// @param ch The chacter to review.
    [[nodiscard]] static inline constexpr bool is_superscript_lowercase(const wchar_t ch) noexcept
        {
        return // a-z
            (ch == 0x207A) ||
            (ch == 0x207B) ||
            (ch == 0x207C) ||
            (ch == 0x207D) ||
            (ch == 0x207E) ||
            (ch == 0x1D43) ||
            (ch == 0x1D47) ||
            (ch == 0x1D9C) ||
            (ch == 0x1D48) ||
            (ch == 0x1d49) ||
            (ch == 0x1DA0) ||
            (ch == 0x1D4D) ||
            (ch == 0x02B0) ||
            (ch == 0x2071) ||
            (ch == 0x02B2) ||
            (ch == 0x1D4F) ||
            (ch == 0x02E1) ||
            (ch == 0x1D50) ||
            (ch == 0x207F) ||
            (ch == 0x1D52) ||
            (ch == 0x1D56) ||
            (ch == 0x02B3) ||
            (ch == 0x02E2) ||
            (ch == 0x1D57) ||
            (ch == 0x1D58) ||
            (ch == 0x1D5B) ||
            (ch == 0x02B7) ||
            (ch == 0x02E3) ||
            (ch == 0x02B8) ||
            (ch == 0x1DBB);
        }

    /// @returns True if @c ch is a superscript.
    /// @param ch The chacter to review.
    [[nodiscard]] static inline constexpr bool is_superscript(const wchar_t ch) noexcept
        {
        return (ch == 0x2070) ||
            (ch == 0xB9) ||
            (ch == 0xB2) ||
            (ch == 0xB3) ||
            (ch == 0x2074) ||
            (ch == 0x2075) ||
            (ch == 0x2076) ||
            (ch == 0x2077) ||
            (ch == 0x2078) ||
            (ch == 0x2079) ||
            // simple math characters
            (ch == 0x207A) ||
            (ch == 0x207B) ||
            (ch == 0x207C) ||
            (ch == 0x207D) ||
            (ch == 0x207E) ||
            is_superscript_lowercase(ch);
        }

    /** Converts a character into its superscript equivalent.
        @param ch The character to convert.
        @returns The character converted into its superscript equivalent, or
                 the original value if it can't be converted.
        @note This only applies to numbers, simple math characters, and a few letters (e.g., 2 -> ²)*/
    [[nodiscard]] static inline constexpr wchar_t to_superscript(const wchar_t ch) noexcept
        {
        return (ch == L'0' || ch == 0xFF10) ? 0x2070 :
            (ch == L'1' || ch == 0xFF11) ? 0xB9 :
            (ch == L'2' || ch == 0xFF12) ? 0xB2 :
            (ch == L'3' || ch == 0xFF13) ? 0xB3 :
            (ch == L'4' || ch == 0xFF14) ? 0x2074 :
            (ch == L'5' || ch == 0xFF15) ? 0x2075 :
            (ch == L'6' || ch == 0xFF16) ? 0x2076 :
            (ch == L'7' || ch == 0xFF17) ? 0x2077 :
            (ch == L'8' || ch == 0xFF18) ? 0x2078 :
            (ch == L'9' || ch == 0xFF19) ? 0x2079 :
            // simple math characters and letters
            (ch == L'+') ? 0x207A :
            (ch == L'-') ? 0x207B :
            (ch == L'=') ? 0x207C :
            (ch == L'(') ? 0x207D :
            (ch == L')') ? 0x207E :
            (ch == L'a') ? 0x1D43 :
            (ch == L'b') ? 0x1D47 :
            (ch == L'c') ? 0x1D9C :
            (ch == L'd') ? 0x1D48 :
            (ch == L'e') ? 0x1D49 :
            (ch == L'f') ? 0x1DA0 :
            (ch == L'g') ? 0x1D4D :
            (ch == L'h') ? 0x02B0 :
            (ch == L'i') ? 0x2071 :
            (ch == L'j') ? 0x02B2 :
            (ch == L'k') ? 0x1D4F :
            (ch == L'l') ? 0x02E1 :
            (ch == L'm') ? 0x1D50 :
            (ch == L'n') ? 0x207F :
            (ch == L'o') ? 0x1D52 :
            (ch == L'p') ? 0x1D56 :
            (ch == L'r') ? 0x02B3 :
            (ch == L's') ? 0x02E2 :
            (ch == L't') ? 0x1D57 :
            (ch == L'u') ? 0x1D58 :
            (ch == L'v') ? 0x1D5B :
            (ch == L'w') ? 0x02B7 :
            (ch == L'x') ? 0x02E3 :
            (ch == L'y') ? 0x02B8 :
            (ch == L'z') ? 0x1DBB :
            ch;
        }

    ///functions not available in ANSI C
    /** Converts an integer value into a string.
        @param value The integer to convert.
        @param out The character buffer to write the integer as a string into. This can be either a char* or wchar_t* buffer.
        @param length The length of the output buffer (in character count).
        @returns 0 on success, -1 on failure. Will fail if the buffer is either invalid
         or not large enough to hold the converted value.*/
    template<typename charT>
    [[nodiscard]] int itoa(long value, charT* out, const size_t length)
        {
        if (length == 0 || out == nullptr)
            { return -1; }
        //space for a negative sign if we need it
        const size_t signPos = value < 0 ? 1 : 0;
        size_t i = 0;
        if (value == 0)
            { out[i++] = 0x30/*zero*/; }
        else if (value < 0)
            {
            out[i++] = 0x2D/*minus sign*/;
            value = -value;
            }
        while (value > 0 && i+1 < length)
            {
            out[i++] = 0x30 + value%10;
            value /= 10;
            }
        /* Not enough space in the buffer or null terminator? Clear out the data that we copied
           into it and return failure.*/
        if (i+1 == length && value > 0)
            {
            std::memset(out,0, length*sizeof(charT));
            return -1;
            }
        out[i] = 0;
        std::reverse<charT*>(out+signPos, out+i);
        return 0;
        }
    
    /** Determines whether a character is a hexadecimal digit (0-9,A-F,a-f).
        @param ch The letter to be analyzed.*/
    template<typename T>
    [[nodiscard]] constexpr bool is_hex_digit(const T ch) noexcept
        {
        return (is_numeric_8bit(static_cast<wchar_t>(ch)) ||
            ((ch >= 0x61/*'a'*/ && ch <= 0x66/*'f'*/) ||
            (ch >= 0x41/*'A'*/ && ch <= 0x46/*'F'*/)) );
        }

    /** Converts string in hex format to int. Default figures out how much of the string
        is a valid hex string, but passing a value to the second parameter overrides this
        and allows you to indicate how much of the string to try to convert.
        @param hexStr The string to convert.
        @param[out] length How much of the string to analyze. The value -1 will tell the function
         to read until there are no more valid hexadecimal digits. Will return the length that was read.
        @returns The value of the string as an integer.*/
    template<typename T>
    [[nodiscard]] int axtoi(const T* hexStr, size_t& length)
        {
        if (hexStr == nullptr || *hexStr == 0 || length == 0)
            {
            length = 0;
            return 0;
            }
        //skip leading 0x
        if (hexStr[0] == 0x30/*0*/ &&
            length != 1 &&
            is_either<T>(hexStr[1], 0x78/*x*/, 0x58/*X*/))
            {
            hexStr += 2;
            //if they specified a length to read then take into account the 0x that we just skipped over
            if (length != static_cast<size_t>(-1) && length >= 2)
                {
                length -= 2;
                if (length == 0)//just a 0x string, then we're done
                    { return 0; }
                }
            }
        if (length == static_cast<size_t>(-1))
            {
            const T* currentPos = hexStr;
            do
                {
                if (currentPos[0] == 0 || !string_util::is_hex_digit(currentPos[0]))
                    { break; }
                }
            while (currentPos++);

            length = currentPos-hexStr;
            //if no valid hex digits then fail and return zero
            if (length == 0)
                { return 0; }
            }
        size_t strPos = 0;
        int intValue = 0;
        // storage for converted values
        auto digits = std::make_unique<int[]>(length+1);
        while (strPos < length)
            {
            if (hexStr[strPos] == 0)
                { break; }
            // 0-9
            if (hexStr[strPos] >= L'0' && hexStr[strPos] <= L'9')
                { digits[strPos] = (hexStr[strPos] & 0x0F); }
            // A-F
            else if ((hexStr[strPos] >= L'a' && hexStr[strPos] <= L'f') ||
                     (hexStr[strPos] >= L'A' && hexStr[strPos] <= L'F') )
                { digits[strPos] = (hexStr[strPos]&0x0F)+9; }
            else
                { break; }
            ++strPos;
            }
        length = strPos; //in case we short circuited
        const size_t count = strPos;
        size_t digitPos = strPos - 1;
        strPos = 0;
        while (strPos < count)
            {
            //shift OR the bits into return value.
            intValue = intValue | (digits[strPos] << (digitPos << 2));
            --digitPos;
            ++strPos;
            }

        return intValue;
        }

    /** Attempts to convert a string @c buffer to a double. Same as wcstod(),
        except it handles thousands separators as well.
        @param buffer The string buffer to read a number from.
        @param[out] endPtr The (optional) pointer in the buffer where the number text ends.
         (Will be the null terminator if the entire string is a number.)
        @returns The string's value converted to a number. (Will be zero if conversion failed.)
        @todo Need to support 0x (hex) strings.*/
    [[nodiscard]] inline double wcstod_thousands_separator(wchar_t const* buffer, wchar_t** endPtr)
        {
        if (!buffer)
            { return 0; }

        // vanilla version of reading as a number
        wchar_t* end(nullptr);
        auto value = std::wcstod(buffer, &end);

        // step over any space like wcstod would have done
        while (buffer < end && std::iswspace(*buffer))
            { ++buffer; }

        // if wcstod ran into what appears to be a thousands separator,
        // then weed those out and try wcstod again
        if (// wcstod actually read a little bit at least
            end > buffer &&
            // but stopped on a thousands separator
            (*end == ',' || *end == '.') &&
            // and what's after the thousands separator is a number
            *(end + 1) != 0 &&
            is_numeric_8bit(*(end + 1)))
            {
            const auto thousandsSep = *end;
            auto realNumberStart = buffer;
            auto realNumberEnd = buffer;
            // scan past any numbers, +/-, and thousands & radix separators
            while (*realNumberEnd != 0 &&
                (is_numeric_8bit(*realNumberEnd) || is_either(*realNumberEnd, L',', L'.') ||
                 is_either(*realNumberEnd, L'+', L'-')))
                { ++realNumberEnd; }
            // copy over the number text from the buffer, but skipping over the thousands separators
            constexpr size_t bufferSize{ 64 };
            wchar_t realNumberStr[bufferSize]{ 0 };
            size_t newNumBufferCounter{ 0 };
            while (realNumberStart < realNumberEnd &&
                   newNumBufferCounter+1 < bufferSize)
                {
                if (*realNumberStart != thousandsSep)
                    { realNumberStr[newNumBufferCounter++] = *realNumberStart; }
                ++realNumberStart;
                }
            end = const_cast<wchar_t*>(realNumberStart);
            // try wcstod again
            value = std::wcstod(realNumberStr, nullptr);
            }
        // set the end to where we read, if caller asked for it
        if (endPtr)
            { *endPtr = end; }

        return value;
        }

    /** Returns the number of characters in the string pointed to by @c str, not including the
        terminating '\0' character, but at most @c maxlen. In doing this, this looks only at
        the first @c maxlen characters in @c str and never beyond @c str+ @c maxlen. This function should be used
        for input that may not be null terminated.
        @param str The string to review.
        @param maxlen The maximum length of the string to scan.
        @returns The valid length of the string or maxlen, whichever is shorter.*/
    template<typename T>
    [[nodiscard]] size_t strnlen(const T* str, const size_t maxlen) noexcept
        {
        if (!str || maxlen == 0)
            { return 0; }
        size_t i;
        for (i = 0; i < maxlen && str[i]; ++i);
        return i;
        }

    /// Search for substring in string (case-insensitive).
    template<typename T>
    [[nodiscard]] const T* stristr(const T* string, const T* strSearch) noexcept
        {
        if (!string || !strSearch || *strSearch == 0)
            { return nullptr; }
        while (*string)
            {
            //compare the characters one at a time
            size_t i = 0;
            for (i = 0; strSearch[i] != 0; ++i)
                {
                if (string[i] == 0)
                    { return nullptr; }
                if (string_util::tolower(strSearch[i]) != string_util::tolower(string[i]) )
                    {
                    ++string;
                    break;
                    }
                }
            // if the substring loop completed then the substring was found
            if (strSearch[i] == 0)
                { return string; }
            }
        return nullptr;
        }

    /** Searches for substring in a larger string (case-insensitively), limiting the search
        to a specified number of characters.*/
    template<typename T>
    [[nodiscard]] const T* strnistr(const T* string, const T* strSearch, const size_t string_len) noexcept
        {
        if (!string || !strSearch || string_len == 0 || *strSearch == 0)
            { return nullptr; }
        for (size_t i = 0; i < string_len; ++i)
            {
            //compare the characters one at a time
            size_t j = 0;
            for (j = 0; strSearch[j] != 0; ++j)
                {
                if ((i+j) >= string_len || string[i+j] == 0)
                    { return nullptr; }
                if (string_util::tolower(strSearch[j]) != string_util::tolower(string[i+j]) )
                    { break; }
                }
            //if the substring loop completed then the substring was found
            if (strSearch[j] == 0)
                { return (string+i); }
            }
        return nullptr;
        }

    /** Search string in reverse for substring.
        @param offset How far we are in the source string already and how far to go back.*/
    template<typename T>
    [[nodiscard]] const T* strrstr(const T* string,
                    const T* search,
                    size_t offset) noexcept
        {
        if (!string || !search)
            { return nullptr; }
        const size_t len = string_util::strlen(search);
        if (len > offset) return nullptr;
        string -= len;
        offset -= len;
        bool fnd = false;
        while (!fnd && offset > 0)
            {
            fnd = true;
            for (size_t i=0; i < len; ++i)
                {
                if (string[i] != search[i])
                    {
                    fnd = false;
                    break;
                    }
                }
            if (fnd) return string;
            --string;
            --offset;
            }
        return nullptr;
        }

    /// Case-insensitive comparison by character count.
    template<typename T>
    [[nodiscard]] int strnicmp(const T* first, const T* second, size_t count) noexcept
        {
        //first check if either of the strings are null
        if (!first && !second)
            { return 0; }
        else if (!first && second)
            { return -1; }
        else if (first && !second)
            { return 1; }

        int f(0), l(0), result(0);

        if (count)
            {
            do
                {
                f = string_util::tolower(*(first++) );
                l = string_util::tolower(*(second++) );
                } while ( (--count) && f && (f == l) );
            result = static_cast<int>(f - l);
            }
        return result;
        }

    /// Case-insensitive comparison.
    template<typename T>
    [[nodiscard]] int stricmp(const T* first, const T* second) noexcept
        {
        //first check if either of the strings are null
        if (!first && !second)
            { return 0; }
        else if (!first && second)
            { return -1; }
        else if (first && !second)
            { return 1; }

        int f(0), l(0);
        do
            {
            f = string_util::tolower(*(first++) );
            l = string_util::tolower(*(second++) );
            } while (f && (f == l) );

        return static_cast<int>(f - l);
        }

    /** Natural order comparison (recognizes numeric strings).
        This will see "2" as being less than "12".
        @param first_string The first string in the comparison.
        @param second_string The second string in the comparison.
        @param case_insensitive Whether the comparison should be case insensitive.
        @returns -1 if the first string is less, 1 if the first string is greater, or 0 if the strings are equal.*/
    template<typename T>
    [[nodiscard]] int strnatordcmp(const T* first_string, const T* second_string, bool case_insensitive = false)
        {
        //first check if either of the strings are null
        if (!first_string && !second_string)
            { return 0; }
        else if (!first_string && second_string)
            { return -1; }
        else if (first_string && !second_string)
            { return 1; }

        size_t first_string_index = 0, second_string_index = 0;
 
        while (true)
            {
            T ch1 = first_string[first_string_index];
            T ch2 = second_string[second_string_index];

            //skip leading spaces
            while (std::iswspace(ch1))
                { ch1 = first_string[++first_string_index]; }

            while (std::iswspace(ch2))
                { ch2 = second_string[++second_string_index]; }

            // process run of digits
            if (is_numeric_8bit(ch1) && is_numeric_8bit(ch2))
                {
                T *firstEnd(nullptr), *secondEnd(nullptr);
                const double firstDouble = wcstod_thousands_separator(first_string+first_string_index, &firstEnd);
                const double secondDouble = wcstod_thousands_separator(second_string+second_string_index, &secondEnd);

                if (firstDouble < secondDouble)
                    { return -1; }
                else if (firstDouble > secondDouble)
                    { return 1; }
                else // numbers are equal
                    {
                    // if this was the end of both strings then they are equal
                    if (*firstEnd == 0 && *secondEnd == 0)
                        { return 0; }
                    /* the first string is done, but there is more to the second string
                       after the number, so first is smaller*/
                    else if (*firstEnd == 0)
                        { return -1; }
                    /* the second string is done, but there is more to the first string
                       after the number, so first is bigger*/
                    else if (*secondEnd == 0)
                        { return 1; }
                    // there is more to both of them, so move the counter and move on
                    else
                        {
                        // if wcstod_thousands_separator() didn't move the pointers,
                        // then we are stuck, so return that they are equal
                        if (static_cast<decltype(first_string_index)>(firstEnd-first_string) == first_string_index &&
                            static_cast<decltype(second_string_index)>(secondEnd-second_string) == second_string_index)
                            { return 0; }
                        first_string_index = (firstEnd - first_string);
                        second_string_index = (secondEnd - second_string);
                        continue;
                        }
                    }
                }

            //if we are at the end of the strings then they are the same
            if (ch1 == 0 && ch2 == 0)
                { return 0; }

            if (case_insensitive)
                {
                ch1 = string_util::tolower(ch1);
                ch2 = string_util::tolower(ch2);
                }

            if (ch1 < ch2)
                { return -1; }
            else if (ch1 > ch2)
                { return 1; }

            ++first_string_index;
            ++second_string_index;
            }
        }

    /// Compare, recognizing numeric strings and ignoring case.
    template<typename T>
    [[nodiscard]] int strnatordncasecmp(const T* a, const T* b)
        { return strnatordcmp(a, b, true); }

    /// Indicates whether a larger strings ends with the specified suffix. Lengths are provided
    /// by the caller for efficiency. This function is case sensitive.
    template<typename T>
    [[nodiscard]] bool has_suffix(const T* text, const size_t text_length, const T* suffix, const size_t suffix_length) noexcept
        {
        if (text == nullptr || suffix == nullptr || text_length <= suffix_length)
            { return false; }
        return (string_util::strnicmp(text+(text_length-suffix_length), suffix, suffix_length) == 0);
        }

    /** Searches for a matching tag, skipping an extra open/close pairs of symbols in between.
        @param stringToSearch The string to search in.
        @param openSymbol The opening symbol.
        @param closeSymbol The closing symbol that we are looking for.
        @param fail_on_overlapping_open_symbol Whether it should immediately return failure if an open
         symbol is found before a matching close symbol.
        @returns A pointer to where the closing tag is, or null if one can't be found.*/
    template<typename T>
    [[nodiscard]] const T* find_matching_close_tag(const T* stringToSearch, const T openSymbol, const T closeSymbol,
                                            const bool fail_on_overlapping_open_symbol = false) noexcept
        {
        if (!stringToSearch)
            { return nullptr; }
        long open_stack = 0;
        while (*stringToSearch)
            {
            if (stringToSearch[0] == openSymbol)
                {
                if (fail_on_overlapping_open_symbol)
                    { return nullptr; }
                ++open_stack;
                }
            else if (stringToSearch[0] == closeSymbol)
                {
                if (open_stack == 0)
                    { return stringToSearch; }
                --open_stack;
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    /// Searches for a matching tag, skipping any extra open/close pairs of symbols in between.
    template<typename T>
    [[nodiscard]] const T* find_matching_close_tag(const T* stringToSearch, const T* openSymbol,
                                            const T* closeSymbol) noexcept
        {
        if (!stringToSearch || !openSymbol || !closeSymbol)
            { return nullptr; }
        const size_t openSymbolLength = string_util::strlen(openSymbol);
        const size_t closeSymbolLength = string_util::strlen(closeSymbol);
        if (openSymbolLength == 0 || closeSymbolLength == 0)
            { return nullptr; }
        long open_stack = 0;
        const T openSymbolFirstCharacter = openSymbol[0];
        const T closeSymbolFirstCharacter = closeSymbol[0];
        while (*stringToSearch)
            {
            //to prevent unnecessary calls to strncmp, we check the current character first
            if (stringToSearch[0] == openSymbolFirstCharacter &&
                string_util::strncmp(stringToSearch,openSymbol,openSymbolLength) == 0)
                {
                ++open_stack;
                stringToSearch += openSymbolLength;
                continue;
                }
            else if (stringToSearch[0] == closeSymbolFirstCharacter &&
                     string_util::strncmp(stringToSearch,closeSymbol,closeSymbolLength) == 0)
                {
                if (open_stack == 0)
                    { return stringToSearch; }
                --open_stack;
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    /// Searches for a matching tag, skipping any extra open/close pairs of symbols in between.
    /// @todo needs unit test
    template<typename T>
    [[nodiscard]] const T* find_unescaped_matching_close_tag(const T* stringToSearch, const T openSymbol,
                                                      const T closeSymbol) noexcept
        {
        assert(openSymbol != closeSymbol);
        if (!stringToSearch || openSymbol == closeSymbol)
            { return nullptr; }
        const T* const originalStart = stringToSearch;
        long open_stack = 0;
        while (*stringToSearch)
            {
            if (stringToSearch[0] == openSymbol &&
                ((stringToSearch == originalStart) ||
                  stringToSearch[-1] != L'\\'))
                {
                ++open_stack;
                ++stringToSearch;
                continue;
                }
            else if (stringToSearch[0] == closeSymbol &&
                ((stringToSearch == originalStart) ||
                  stringToSearch[-1] != L'\\'))
                {
                if (open_stack == 0)
                    { return stringToSearch; }
                --open_stack;
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    ///@todo needs unit tests
    template<typename T>
    [[nodiscard]] const T* find_unescaped_char(const T* stringToSearch, const T ch) noexcept
        {
        if (!stringToSearch)
            { return nullptr; }
        while (*stringToSearch)
            {
            // if on an escape character, then step over that and whatever it's escaping
            if (*stringToSearch == L'\\')
                {
                stringToSearch += 2;
                continue;
                }
            if (*stringToSearch == ch)
                { break; }
            ++stringToSearch;
            }
        return (*stringToSearch == 0) ? nullptr : stringToSearch;
        }
   
    /** Searches for a single character in a string for n number of characters.
        Size argument should be less than or equal to the length of the string being searched.
        @param stringToSearch The string to search in.
        @param ch The character to search for.
        @param numberOfCharacters The number of characters to search through in the string.
        @returns A pointer in the string where the character was found, or nullptr if not found.*/
    template<typename T>
    [[nodiscard]] const T* strnchr(const T* stringToSearch, const T ch, size_t numberOfCharacters) noexcept
        {
        if (!stringToSearch)
            { return nullptr; }
        size_t i = 0;
        for (i = 0; i < numberOfCharacters; ++i)
            {
            /* if string being searched is shorter than the size argument then return failure (nullptr).*/
            if (stringToSearch[i] == 0)
                { return nullptr; }
            if (stringToSearch[i] == ch)
                { return stringToSearch+i; }
            }
        return nullptr;
        }

    /** Searches in a string for a single character from a larger sequence and returns a pointer if found.
        @param stringToSearch The string to search in.
        @param searchSequence The sequence of characters to search for. If any character in this sequence
         is found in @c stringToSearch, then its position will be returned.
        @param searchSeqLength The length of the search sequence.
        @returns A pointer to where the character was found, or null if not found.*/
    template<typename T>
    [[nodiscard]] const T* strcspn_pointer(const T* stringToSearch, const T* searchSequence, const size_t searchSeqLength) noexcept
        {
        if (!stringToSearch || !searchSequence || searchSeqLength == 0)
            { return nullptr; }
        assert((string_util::strlen(searchSequence) == searchSeqLength) && "Invalid length passed to strcspn_pointer().");
        size_t i = 0;
        while (stringToSearch)
            {
            if (stringToSearch[0] == 0)
                { return nullptr; }
            for (i = 0; i < searchSeqLength; ++i)
                {
                if (stringToSearch[0] == searchSequence[i])
                    { return stringToSearch; }
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    /** Searches for a single character from a sequence in a string for n number of characters.
        @param stringToSearch The string to search.
        @param stringToSearchLength The length of the string being searched.
        @param searchString The sequence of characters to search for.
        @param searchStringLength The length of the sequence string.
        @returns The index into the string that the character was found. Returns the length of the string if not found.*/
    template<typename T>
    [[nodiscard]] size_t strncspn(const T* stringToSearch, const size_t stringToSearchLength,
        const T* searchString, const size_t searchStringLength) noexcept
        {
        if (!stringToSearch || !searchString || stringToSearchLength == 0 || searchStringLength == 0)
            { return stringToSearchLength; }
        assert((string_util::strlen(searchString) == searchStringLength) && "Invalid length passed to strncspn().");
        size_t i = 0, j = 0;
        for (i = 0; i < stringToSearchLength; ++i)
            {
            /* If string being searched is shorter than the size argument then fail.
               Note that we should pass back the length of the string that the caller passed in
               so that they know that it failed, not the actual position that we stopped on which would
               be a null terminator.*/
            if (stringToSearch[i] == 0)
                { return stringToSearchLength; }
            for (j = 0; j < searchStringLength; ++j)
                {
                if (stringToSearch[i] == searchString[j])
                    { return i; }
                }
            }
        return i;
        }

    /** Searches for a single character not from a sequence in a string in reverse.
        @param string The string to search in.
        @param search The sequence of characters to skip.
        @param offset Where to begin the search. If -1, then the reverse search will begin at the end of the string.
        @returns The position of where the last non-matching character is at, or -1 if it can't be found.*/
    template<typename T>
    [[nodiscard]] size_t find_last_not_of(const T* string, const T* search,
                    size_t offset = std::basic_string<T>::npos) noexcept
        {
        if (!string || !search)
            { return std::basic_string<T>::npos; }
        if (offset == std::basic_string<T>::npos)
            {
            offset = string_util::strlen(string);
            if (offset == 0)
                { return std::basic_string<T>::npos; }
            --offset;
            }
        assert(offset < string_util::strlen(string));

        size_t i=0;
        while (offset != std::basic_string<T>::npos)
            {
            for (i = 0; search[i] != 0; ++i)
                {
                if (string[offset] == search[i])
                    { break; }
                }
            //If the whole search string wasn't scanned, then there was a match. Move on.
            if (search[i] != 0)
                {
                if (offset == 0)
                    { return std::basic_string<T>::npos; }
                --offset;
                }
            else
                { return offset; }
            }
        return std::basic_string<T>::npos;
        }

    /** Searches for the last instance of a character in a string in reverse.
        @param string The string to search.
        @param ch The character to search for.
        @param offset The offset in the string to begin the search from. The default (-1) will begin the search at the end of the string.
        @returns The offset of the found character, or -1 if not found.*/
    template<typename T>
    [[nodiscard]] size_t find_last_of(const T* string,
                    const T ch,
                    size_t offset = -1) noexcept
        {
        if (!string)
            { return static_cast<size_t>(-1); }
        if (offset == std::basic_string<T>::npos)
            {
            offset = string_util::strlen(string);
            if (offset == 0)
                { return std::basic_string<T>::npos; }
            --offset;
            }
        assert(offset < string_util::strlen(string));

        while (offset != static_cast<size_t>(-1))
            {
            if (string[offset] == ch)
                { return offset; }
            --offset;
            }
        return static_cast<size_t>(-1);
        }

    /** Searches for a single character from a sequence in a string in reverse.
        @param string The string to search in.
        @param search The sequence of characters to skip.
        @param offset Where to begin the search. If -1, then the reverse search will begin at the end of the string.
        @returns The position of where the last matching character is at, or -1 if it can't be found.*/
    template<typename T>
    [[nodiscard]] size_t find_last_of(const T* string, const T* search,
                    size_t offset = std::basic_string<T>::npos) noexcept
        {
        if (!string || !search)
            { return std::basic_string<T>::npos; }
        if (offset == std::basic_string<T>::npos)
            {
            offset = string_util::strlen(string);
            if (offset == 0)
                { return std::basic_string<T>::npos; }
            --offset;
            }
        assert(offset < string_util::strlen(string));

        size_t i=0;
        while (offset != std::basic_string<T>::npos)
            {
            for (i = 0; search[i] != 0; ++i)
                {
                if (string[offset] == search[i])
                    { break; }
                }
            //If the whole search string wasn't scanned, then there was a match. Stop.
            if (search[i] != 0)
                { return offset; }
            else
                {
                if (offset == 0)
                    { return std::basic_string<T>::npos; }
                --offset;
                }
            }
        return std::basic_string<T>::npos;
        }

    /* Searches for the first occurrence that is not a character from a sequence in
       a string for n number of characters and returns zero-based index if found.
       @param stringToSearch The string to search.
       @param stringToSearchLength The length of the string being searched.
       @param searchString The sequence of characters to perform a reverse match with.
       @param searchStringLength The length of the search character sequence.
       @returns The index into the string that the character was not found, or 
        the length of the string if nothing was found that couldn't match the search string.*/
    template<typename T>
    [[nodiscard]] size_t find_first_not_of(const T* stringToSearch, const size_t stringToSearchLength,
                                    const T* searchString, const size_t searchStringLength) noexcept
        {
        if (!stringToSearch || !searchString || stringToSearchLength == 0 || searchStringLength == 0)
            { return stringToSearchLength; }
        size_t i = 0, j = 0;
        for (i = 0; i < stringToSearchLength; ++i)
            {
            /* If string being searched is shorter than the size argument then return string length to indicate failure to find anything.
               Although this isn't really the index that we stopped in, this will tell the caller that the search failed.*/
            if (stringToSearch[i] == 0)
                { return stringToSearchLength; }
            for (j = 0; j < searchStringLength; ++j)
                {
                //stop if a character matching anything in the sequence
                if (stringToSearch[i] == searchString[j])
                    { break; }
                }
            //if we went through the whole sequence then nothing matched
            if (j == searchStringLength)
                { return i; }
            }
        return i;
        }

    /// Searches for a string in a larger string as a whole word.
    /// @param haystack The string to search inside of.
    /// @param needle The string to search for.
    /// @param index The starting position within @haystack to start the search.
    template<typename T>
    [[nodiscard]] typename T::size_type find_whole_word(const T& haystack,
                                        const T& needle,
                                        size_t start_index = 0)
        {
        if (needle.length() == 0 || haystack.length() == 0)
            { return T::npos; }

        auto start{ start_index };

        while (start != T::npos)
            {
            start = haystack.find(needle, start);

            if (start == T::npos)
                { return T::npos; }
            // if at start of haystack
            else if (start == 0)
                {
                if (needle.length() == haystack.length() )
                    { return start; }
                if (std::iswspace(haystack[start+needle.length()]) ||
                    std::iswpunct(haystack[start+needle.length()]))
                    { return start; }
                else
                    {
                    ++start;
                    continue;
                    }
                }
            // at end of haystack
            else if ((start + needle.length() ) == haystack.length() )
                {
                if (needle.length() == haystack.length() )
                    { return start; }
                if (std::iswspace(haystack[start-1]) ||
                    std::iswpunct(haystack[start-1]))
                    { return start; }
                else
                    {
                    ++start;
                    continue;
                    }
                }
            // inside of haystack
            else
                {
                if (needle.length() == haystack.length() )
                    { return start; }
                if ((std::iswspace(haystack[start+needle.length()]) ||
                     std::iswpunct(haystack[start+needle.length()])) &&
                    (std::iswspace(haystack[start-1]) ||
                     std::iswpunct(haystack[start-1])) )
                    { return start; }
                else
                    {
                    ++start;
                    continue;
                    }
                }
            }
        return T::npos;
        }

    //utility classes
    //equal functors
    template<typename T>
    class equal_string_i_compare
        {
    public:
        [[nodiscard]] bool operator()(const T* a_, const T* b_) const noexcept
            { return (string_util::stricmp(a_, b_) == 0); }
        };

    template<typename T>
    class equal_basic_string_i_compare
        {
    public:
        [[nodiscard]] bool operator()(const T& a_, const T& b_) const noexcept
            { return (string_util::stricmp(a_.c_str(), b_.c_str()) == 0); }
        };

    /// Case-insensitive predicate for comparing basic_string types against
    /// strings in a std::map (usually in conjunction with std::find_if()).
    template<typename TKey, typename TVal>
    class equal_basic_string_i_compare_map
        {
    public:
        /// @brief Constructor.
        /// @param key The value to compare against.
        equal_basic_string_i_compare_map(const TKey& key) noexcept
            { m_key = key; }
        /// @returns true if initial value is the same the pair's key.
        /// @param val The pair to compare against.
        [[nodiscard]] bool operator()(const std::pair<TKey,TVal>& val) const noexcept
            { return (string_util::stricmp(val.first.c_str(), m_key.c_str()) == 0); }
    private:
        TKey m_key;
        };

    template<typename T>
    class equal_string_compare
        {
    public:
        [[nodiscard]] bool operator()(const T* a_, const T* b_) const noexcept
            { return (string_util::strcmp(a_, b_) == 0); }
        };

    //less functors
    template<typename T>
    class less_string_n_compare
        {
    public:
        less_string_n_compare(size_t comparison_size) : m_comparison_size(comparison_size) {}
        [[nodiscard]] bool operator()(const T* a_, const T* b_) const noexcept
            { return (string_util::strncmp(a_, b_, m_comparison_size) < 0); }
    private:
        size_t m_comparison_size{ 0 };
        };

    template<typename T>
    class less_string_ni_compare
        {
    public:
        less_string_ni_compare(size_t comparison_size) : m_comparison_size(comparison_size) {}
        [[nodiscard]] bool operator()(const T* a_, const T* b_) const noexcept
            { return (string_util::strnicmp(a_, b_, m_comparison_size) < 0); }
    private:
        size_t m_comparison_size{ 0 };
        };

    template<typename T>
    class less_string_i_compare
        {
    public:
        [[nodiscard]] bool operator()(const T* a_, const T* b_) const
            { return (string_util::stricmp(a_, b_) < 0); }
        };

    template<typename T>
    class less_string_compare
        {
    public:
        [[nodiscard]] bool operator()(const T* a_, const T* b_) const
            { return (string_util::strcmp(a_, b_) < 0); }
        };

    template<typename T>
    class less_basic_string_compare
        {
    public:
        [[nodiscard]] bool operator()(const T& a_, const T& b_) const noexcept
            { return (string_util::strcmp(a_.c_str(), b_.c_str()) < 0); }
        };

    template<typename T>
    class less_basic_string_i_compare
        {
    public:
        [[nodiscard]] bool operator()(const T& a_, const T& b_) const noexcept
            { return (string_util::stricmp(a_.c_str(), b_.c_str()) < 0); }
        };

    template<typename T>
    class less_string_natural_order_i_compare
        {
    public:
        [[nodiscard]] bool operator()(const T* a_, const T* b_) const
            { return (string_util::strnatordncasecmp(a_, b_) < 0); }
        };

    /** Performs a heuristic check on a buffer to see if it's 7-bit or 8-bit ASCII.
        @param buffer The buffer to review (should be either char or unsigned char.
        @param buffSize The byte count of \c buffer.
        @returns True if \c buffer is some sort of ASCII, false if possibly UTF-16.
        @note The larger the buffer, the more accurate the check will be.*/
    template<typename T>
    [[nodiscard]] bool is_extended_ascii(const T* buffer, const size_t buffSize) noexcept
        {
        static_assert(std::is_same<T, char>::value || std::is_same<T, unsigned char>::value);
        if (!buffer || buffSize == 0)
            { return false; }

        size_t spaceCount{0};

        // go up to last 2 characters to determine what we are looking at
        for (size_t i = 0; i < buffSize-1; ++i)
            {
            // embedded NULL terminator midstream? More than likely that this is some variant of UTF
            // and this char sequence we are on is a Unicode char with a zero low or high bit
            if (buffer[i] == 0 && buffer[i+1] != 0)
                { return false; }
            else if (std::isspace(static_cast<unsigned char>(buffer[i])))
                { ++spaceCount; }
            }

        // No embedded zeros, so this likely is 7-bit/8-bit,
        // unless a large buffer with no ASCII spaces. No spaces
        // in a large block of text is not normal for non-CJK text.
        return (buffSize > 128) ? (spaceCount > 0) : true;
        }

    /// @brief Trims left side of @c str (in-place)
    /// @note This assumes the string type uses wchar_t as its datatype.
    template<typename string_typeT>
    void ltrim(string_typeT& str)
        {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(),
            [](wchar_t ch) noexcept
                { return !std::iswspace(ch); }
            ));
        }

    /// @brief Trims right side of @c str (in-place)
    /// @note This assumes the string type uses wchar_t as its datatype.
    template<typename string_typeT>
    void rtrim(string_typeT& str)
        {
        str.erase(std::find_if(str.rbegin(), str.rend(),
            [](wchar_t ch) noexcept
                { return !std::iswspace(ch); }
            ).base(), str.end());
        }

    /// @brief Trims left and right sides of @c str (in-place)
    /// @note This assumes the string type uses wchar_t as its datatype.
    template<typename string_typeT>
    void trim(string_typeT& s)
        { ltrim(s); rtrim(s); }

    /// @brief Trims punctuation from left side of @c str (in-place).
    /// @note This assumes the string type uses wchar_t as its datatype.
    template<typename string_typeT>
    void ltrim_punct(string_typeT& str)
        {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(),
            [](wchar_t ch)
                { return !std::iswpunct(ch); }
            ));
        }

    /// @brief Trims punctuation from right side of @c str (in-place).
    /// @note This assumes the string type uses wchar_t as its datatype.
    template<typename string_typeT>
    void rtrim_punct(string_typeT& str)
        {
        str.erase(std::find_if(str.rbegin(), str.rend(),
            [](wchar_t ch)
                { return !std::iswpunct(ch); }
            ).base(), str.end());
        }

    /// @brief Trims punctuation from left and right sides of @c str (in-place).
    /// @note This assumes the string type uses wchar_t as its datatype.
    template<typename string_typeT>
    void trim_punct(string_typeT& str)
        { ltrim_punct(str); rtrim_punct(str); }

    /// @brief Trims whitespace from around a string.
    template<typename char_typeT>
    class string_trim
        {
    public:
        /** @returns Position into the string buffer where the first non-space is.
            @param value The string to trim.
            @param length The length of @c value.
            Call get_trimmed_string_length() to see how much to read from there to
            see where the last non-space is at the end.*/
        [[nodiscard]] const char_typeT* operator()(const char_typeT* value,
                                     size_t length = std::basic_string<char_typeT>::npos) noexcept
            {
            m_trimmed_string_length = 0;
            if (value == nullptr)
                { return nullptr; }
            if (length == 0)
                { return value; }
            if (length == std::basic_string<char_typeT>::npos)
                { length = string_util::strlen(value); }
            const char_typeT* start = value;
            //end is last valid character in the string, not the one after it
            const char_typeT* end = value+(length-1);
            while (start && (start <= end))
                {
                if (std::iswspace(start[0]))
                    { ++start; }
                else
                    { break; }
                }
            while (end > start)
                {
                if (std::iswspace(end[0]))
                    { --end; }
                else
                    { break; }
                }
            //if start overran end then this string was all spaces.
            m_trimmed_string_length = (start > end) ? 0 : (end-start)+1;
            return start;
            }
        /// @returns The length of the string buffer, ignoring spaces on the left and right.
        [[nodiscard]] size_t get_trimmed_string_length() const noexcept
            { return m_trimmed_string_length; }
    private:
        size_t m_trimmed_string_length{ 0 };
        };

    /** \addtogroup StringOperations
    * Classes for string operations.
    * @{*/
    /**
    @class string_tokenize
    @brief Tokenizes a string using a set of delimiters.
    @date 2010
    */
    /** @} */
    template<typename T>
    class string_tokenize
        {
    public:
        /// Constructor which takes the string to parse and the delimiters to use.
        /// @param val The string to parse.
        /// @param delim The set of delimiters to separate the string.
        string_tokenize(const T& val, const T& delim) noexcept :
            m_value(val), m_start(nullptr), m_next_delim(nullptr), m_delim(delim), m_has_more_tokens(true)
            {
            m_start = m_value.c_str();
            m_next_delim = string_util::strcspn_pointer(m_start, m_delim.c_str(), m_delim.length());
            }
        /// @returns Whether or not there are more tokens in the string.
        [[nodiscard]] bool has_more_tokens() const noexcept
            { return m_has_more_tokens; }
        /// @returns Whether or not there are more delimiters in the string.
        /// This is useful for seeing if there are any delimiters at all when first loading the string.
        [[nodiscard]] bool has_more_delimiters() const noexcept
            { return (m_next_delim != nullptr); }
        /// @returns The next token from the original string as a string object
        /// @note Empty tokens can be returned if there is proceeding or trailing
        /// delimiters in the string, or if there are repeated delimiters next to each other.
        [[nodiscard]] T get_next_token()
            {
            if (m_next_delim)
                {
                const wchar_t* current_start = m_start;
                const wchar_t* current_next_delim = m_next_delim;
                //move the pointers to the next token
                m_start = ++m_next_delim;
                m_next_delim = string_util::strcspn_pointer(m_start, m_delim.c_str(), m_delim.length());
                return T(current_start, current_next_delim-current_start);
                }
            //no more delims means that we are on the last token
            else if (m_start)
                {
                m_has_more_tokens = false;
                const wchar_t* current_start = m_start;
                m_start = nullptr;
                return T(current_start);
                }
            //if called when there are no more tokens, then return an empty string
            else
                {
                m_has_more_tokens = false;
                return T();
                }
            }
    private:
        string_tokenize() = delete;
        string_tokenize(const string_tokenize&) = delete;

        T m_value;
        const wchar_t* m_start{ nullptr };
        const wchar_t* m_next_delim{ nullptr };
        T m_delim;
        bool m_has_more_tokens{ false };
        };

    ///Removes all whitespace from a string
    template<typename T>
    [[nodiscard]] T remove_all_whitespace(const T& text)
        {
        T tempText = text;
        for (typename T::size_type i = 0; i < tempText.length(); /*in loop*/)
            {
            if (tempText[i] == 10 || tempText[i] == 13 || tempText[i] == 9)
                { tempText.erase(i,1); }
            else
                { ++i; }
            }
        return tempText;
        }

    /// Removes all instances of a character from a string.
    template<typename T>
    void remove_all(T& text, const typename T::traits_type::char_type char_to_replace)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(char_to_replace, start);
            if (start == T::npos)
                { return; }
            text.erase(start,1);
            }
        }

    /** Replace all instances of a character in a string.
        @param text The text to replace items in.
        @param charToReplace The character to replace.
        @param replacementChar The character to replace @c charToReplace with.*/
    template<typename T>
    void replace_all(T& text,
                     const typename T::traits_type::char_type charToReplace,
                     const typename T::traits_type::char_type replacementChar)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(charToReplace, start);
            if (start == T::npos)
                { return; }
            text[start++] = replacementChar;
            }
        }

    /** Replace all instances of a substring in a string.
        @param text The text to replace items in.
        @param textToReplace The text to replace.
        @param textToReplaceLength The length of @c textToReplace.
        @param replacementText The text to replace @c textToReplace with.*/
    template<typename T>
    void replace_all(T& text,
                     const typename T::traits_type::char_type* textToReplace,
                     const size_t textToReplaceLength,
                     const typename T::traits_type::char_type* replacementText)
        {
        if (!textToReplace || !replacementText)
            { return; }
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(textToReplace, start);
            if (start == T::npos)
                { return; }
            text.replace(start, textToReplaceLength, replacementText);
            }
        }

    /** Replace all instances of a substring in a string.
        @param text The text to replace items in.
        @param textToReplace The text to replace.
        @param replacementText The text to replace @c textToReplace with.*/
    template<typename T>
    void replace_all(T& text, const T& textToReplace, const T& replacementText)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(textToReplace, start);
            if (start == T::npos)
                { return; }
            text.replace(start, textToReplace.length(), replacementText);
            }
        }

    /** Replace all instances of a substring in a string (searching by whole word).
        @param text The text to replace items in.
        @param textToReplace The text to replace.
        @param replacementText The text to replace @c textToReplace with.
        @param index Where to start the search within @c text.*/
    template<typename T>
	void replace_all_whole_word(T& text, const T& textToReplace, 
							    const T& replacementText, 
							    const size_t index = 0)
		{
		if (textToReplace.length() == 0 || replacementText.length() == 0)
			{ return; }
        auto start{ index };

		while (start != T::npos)
			{
			start = find_whole_word(text, textToReplace, start);
			if (start == T::npos)
				{ break; }
			text.replace(start, textToReplace.length(), replacementText);
			start += replacementText.length();
			}
		}

    /** Strips extraneous spaces/tabs/carriage returns from a block of text so
        that there isn't more than one space consecutively.*/
    template<typename string_typeT>
    size_t remove_extra_spaces(string_typeT& Text)
        {
        size_t numberOfSpacesRemoved = 0;

        if (!Text.length() )
            { return 0; }
        bool alreadyHasSpace = true;
        //make sure that there is only a space between each word
        for (unsigned int i = 0; i < Text.length(); ++i)
            {
            //if this is the first space found after the current
            //word then it's OK--just leave it
            if (std::iswspace(Text[i]) && !alreadyHasSpace)
                { alreadyHasSpace = true; }
            //this is extra space right after another--get rid of it
            else if (std::iswspace(Text[i]) && alreadyHasSpace)
                {
                //make sure it isn't a Windows \r\n
                if (i && !(Text[i-1] == 13 && Text[i] == 10))
                    {
                    ++numberOfSpacesRemoved;
                    Text.erase(i--,1);
                    }
                }
            //we are starting another word--reset
            else if (!std::iswspace(Text[i]))
                { alreadyHasSpace = false; }
            }

        return numberOfSpacesRemoved;
        }

    /** Removes blank lines from a block of text (in-place).
        @param Text The text to have blank lines removed from.
        @returns The number of characters (not lines) removed from the block.*/
    template<typename string_typeT>
    size_t remove_blank_lines(string_typeT& Text)
        {
        size_t numberOfLinesRemoved = 0;

        if (!Text.length() )
            { return 0; }
        bool alreadyHasNewLine = true;
        //make sure that there is only a space between each word
        for (unsigned int i = 0; i < Text.length(); ++i)
            {
            //if this is the first space found after the current
            //word then it's OK--just leave it
            if (is_either<wchar_t>(static_cast<wchar_t>(Text[i]), 10, 13) && !alreadyHasNewLine)
                { alreadyHasNewLine = true; }
            //this is extra space right after another--get rid of it
            else if (is_either<wchar_t>(static_cast<wchar_t>(Text[i]), 10, 13) && alreadyHasNewLine)
                {
                //make sure it isn't a Windows \r\n
                if (i && !(Text[i-1] == 13 && Text[i] == 10))
                    {
                    ++numberOfLinesRemoved;
                    Text.erase(i--,1);
                    }
                }
            //we are starting another word--reset
            else if (!is_either<wchar_t>(static_cast<wchar_t>(Text[i]), 10, 13))
                { alreadyHasNewLine = false; }
            }

        return numberOfLinesRemoved;
        }

    /** Converts strings to double values, but also takes into account ranges (returning the average). For example, a string
        like "5-8" will return 6.5. Hyphens and colons are seen as range separators.*/
    template<typename Tchar_type>
    [[nodiscard]] double strtod_ex(const Tchar_type* nptr, Tchar_type** endptr) noexcept
        {
        if (nptr == nullptr)
            {
            *endptr = nullptr;
            return 0.0f;
            }
        const wchar_t sepStr[3] = { 0x2D, 0x3A, 0 };
        const Tchar_type* separator = string_util::strcspn_pointer<Tchar_type>(nptr, sepStr, 2);
        // if there is no hyphen or there is one but it is at the end then just call strtod
        if (separator == nullptr || *(separator+1) == 0)
            { return string_util::strtod(nptr, endptr); }
        else
            {
            const double d1 = string_util::strtod(nptr, endptr);
            const double d2 = string_util::strtod(++separator, endptr);
            return (d1+d2) / static_cast<double>(2);
            }
        }

    /** Determines if a character is one of a list of characters.
        @param character The character to review.
        @param char_string The list of characters to compare against.
        @returns True if the character of one of the list of characters.*/
    template<typename Tchar_type>
    [[nodiscard]] constexpr bool is_one_of(const Tchar_type character, const Tchar_type* char_string) noexcept
        {
        if (char_string == nullptr)
            { return false; }
        while (*char_string)
            {
            if (character == char_string[0])
                { return true; }
            ++char_string;
            }
        return false;
        }

    /** Converts a full-width number/English letter/various symbols into its "narrow" counterpart.
        @param ch The character to convert.
        @returns The narrow version of a character, or the character if not full-width.
        @todo Add support for wide Japanese/Korean characters.*/
    [[nodiscard]] inline constexpr wchar_t full_width_to_narrow(const wchar_t ch) noexcept
        {
        return (ch >= 65'281 && ch <= 65'374) ? (ch - 65'248) :
            // cent and pound sterling
            (ch >= 65'504 && ch <= 65'505) ? (ch - 65'342) :
            // Yen
            (ch == 65'509) ? 165 :
            // Not
            (ch == 65'506) ? 172 :
            // macron
            (ch == 65'507) ? 175 :
            // broken bar
            (ch == 65'508) ? 166 :
            ch;
        }
    }

/** @}*/

#endif //__STRING_UTIL_H__
