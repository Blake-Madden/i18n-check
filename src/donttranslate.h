/** @addtogroup Internationalization
    @brief Classes for reviewing code for internationalization issues.
    @date 2021
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
    it under the terms of the BSD License.
* @{*/

#ifndef __DONTTRANSLATE_H__
#define __DONTTRANSLATE_H__

#include <type_traits>

// Determines whether T is string constant type.
template<class T>
struct is_string_constant
    : std::bool_constant<std::is_same_v<T, const char*> ||
                         std::is_same_v<T, const wchar_t*> ||
                         std::is_same_v<T, const uint8_t*> ||
                         std::is_same_v<T, const char16_t*> ||
                         std::is_same_v<T, const char32_t*>>
    {};

// Helper for is_string_constant.
template<class _Ty>
inline constexpr bool is_string_constant_v = is_string_constant<_Ty>::value;

/// Explanations for why a string should not be available for translation.
enum class DTExplanation
    {
    DebugMessage,    /*!< Debugging/Tracing related string. */
    LogMessage,      /*!< Log messages that aren't normally user facing. */
    ProperNoun,      /*!< The name of a proper person, place, or thing that wouldn't normally be translated. */
    FilePath,        /*!< A filename or path. */
    InternalKeyword, /*!< An internal keyword or constant. */
    Command,         /*!< A command, such as "open" in a `::ShellExecute()` call. */
    SystemEntry,     /*!< A system entry, such as an entry in the registry. */
    FormatString,    /*!< A printf format string. */
    Syntax,          /*!< Any sort of code or formula. */
    NoExplanation    /*!< No explanation. */
    };

/** @brief "Don't Translate." Simply expands a string in place, indicating to the developer that is not meant to be translated.

     This is useful for explicitly stating that a string is not meant for translation.

     In essense, this is the opposite of the `_()` macro from the **GETTEXT** library that marks a string as translatable.
    @param str The string.
    @param explanation An optional type of explanation for why this string should not be available for translation.
    @param customMessage An optional message to add explaining why this shouldn't be translated. This is a useful
                         alternative to wrapping comments around the code.
    @returns The same string.
    @note This works with `char`, `uint8_t`, `char16_t`, `char32_t`, and `wchar_t` type string constants.
    @sa _DT().
    @par Example
    @code
        const std::string fileName = "C:\\data\\logreport.txt";

        // "open " should not be translated, it's part of a command line
        auto command = DONTTRANSLATE("open ") + fileName;
        // expands to "open C:\\data\\logreport.txt"

        // a more descriptive approach
        auto command2 = DONTTRANSLATE("open ", DTExplanation::Command) + fileName;
        // also expands to "open C:\\data\\logreport.txt"

        // an even more descriptive approach
        auto command3 = DONTTRANSLATE("open ",
                                      DTExplanation::Command,
                                      "This is part of a command line, don't expose for translation!") +
                        fileName;
        // also expands to "open C:\\data\\logreport.txt"

        // a shorthand, _DT(), is also available
        auto command = _DT("open ") + fileName;
    @endcode*/
template<typename T,
         std::enable_if_t<is_string_constant_v<T>, bool> = true>
inline constexpr auto DONTTRANSLATE(T str,
                  [[maybe_unused]] const DTExplanation explanation = DTExplanation::NoExplanation,
                  [[maybe_unused]] T customMessage = nullptr)
    { return str; }

/** @brief A shorthand alias for DONTTRANSLATE().
    @param str The string.
    @param explanation An optional type of explanation for why this string should not be available for translation.
    @param customMessage An optional message to add explaining why this shouldn't be translated.
    @returns The same string.*/
template<typename T,
         std::enable_if_t<is_string_constant_v<T>, bool> = true>
inline constexpr auto _DT(T str,
                  [[maybe_unused]] const DTExplanation explanation = DTExplanation::NoExplanation,
                  [[maybe_unused]] T customMessage = nullptr)
    { return str; }

/** @}*/

#endif //__DONTTRANSLATE_H__
