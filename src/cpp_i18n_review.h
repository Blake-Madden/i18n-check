/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __CPP_I18N_REVIEW_H__
#define __CPP_I18N_REVIEW_H__

#include "i18n_review.h"
#include <map>
#include <set>
#include <span>
#include <string>
#include <utility>
#include <vector>

/// @brief Classes for checking source code for internationalization/localization issues.
namespace i18n_check
    {
    /** @brief Class to extract and review localizable/nonlocalizable text from C++ source code.*/
    class cpp_i18n_review : public i18n_review
        {
      public:
        /** @brief Main interface for extracting resource text from C++ source code.
            @param srcText The C++ code text to extract text from.
            @param fileName The (optional) name of source file being analyzed.*/
        void operator()(std::wstring_view srcText, const std::wstring& fileName) final;

      private:
        /// @brief Strips off the trailing template and global accessor (i.e., "::")
        ///     information from a function/variable.
        void remove_decorations(std::wstring& str) const override;

        /// @returns @c true if provided variable type is just a decorator after the real
        ///     variable type (e.g., const) and should be skipped.
        /// @param variableType The parsed variable type to review.
        [[nodiscard]]
        bool is_variable_type_decorator(const std::wstring_view variableType) const final
            {
            return variableType == L"const";
            }

        /// @returns The end of a raw string.
        /// @param text The current string to parse.
        /// @param chr The tag indicating what type of raw string this is.
        [[nodiscard]]
        virtual wchar_t* find_raw_string_end(wchar_t* text,
                                             [[maybe_unused]] const wchar_t chr) const
            {
            return std::wcsstr(text, L")\"");
            }

        /// @returns How many characters a to step into/out of a raw string.
        ///     In other words, how many characters are inside of the string marking the
        ///     sentinel boundaries of the raw content.
        /// @param chr The tag indicating what type of raw string this is.
        [[nodiscard]]
        virtual size_t get_raw_step_size([[maybe_unused]] const wchar_t chr) const
            {
            return 1;
            }

        /// @return @c true if a character indicates a raw string.
        /// @param chr The character in front of the string.
        [[nodiscard]]
        virtual bool is_raw_string_marker(const wchar_t chr) const
            {
            return chr == L'R';
            }

        /// @returns Assuming that we are at the first character of a raw string, return where the
        ///     actual start of the text is.
        /// @param text The current string to parse.
        /// @param chr The tag indicating what type of raw string this is.
        [[nodiscard]]
        wchar_t* raw_step_into_string(wchar_t* text, const wchar_t chr) const
            {
            return std::next(text, get_raw_step_size(chr));
            }

        /// @brief Parses and processes a preprocessor directive.
        /// @param directiveStart The start of the preprocessor section.
        /// @param directivePos The position in the overall text that the preprocessor block is at.
        /// @returns The end of the current preprocessor block.
        [[nodiscard]]
        wchar_t* process_preprocessor_directive(wchar_t* directiveStart, const size_t directivePos);

        /// @brief Skips a preprocessor define section.
        /// @details These will be debug preprocessor sections that we won't want to analyze.
        /// @param directiveStart The start of the preprocessor section.
        /// @returns The end of the current preprocessor block.
        [[nodiscard]]
        static wchar_t* skip_preprocessor_define_block(wchar_t* directiveStart);

        /// @brief Parses and processes an inline assembly block.
        /// @param asmStart The start of the asm section.
        /// @returns The end of the current asm block.
        [[nodiscard]]
        wchar_t* process_assembly_block(wchar_t* asmStart);

        /// @returns @c true if text is an inline assembly block.
        [[nodiscard]]
        bool is_assembly_block(std::wstring_view text) const noexcept
            {
            assert(!text.empty());
            if (text.empty())
                {
                return false;
                }
            return (text.starts_with(L"asm ") || text.starts_with(L"__asm ") ||
                    (text.length() >= 7 && text.starts_with(L"__asm__") &&
                     (std::iswspace(text[7]) || text[7] == L'(')));
            }
        };
    } // namespace i18n_check

/** @}*/

#endif //__CPP_I18N_REVIEW_H__
