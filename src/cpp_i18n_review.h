/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2023
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __CPP_I18N_REVIEW_H__
#define __CPP_I18N_REVIEW_H__

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include "i18n_review.h"

/// @brief Classes for checking source code for internationalization/localization issues.
namespace i18n_check
    {
    /** @brief Class to extract and review localizable/nonlocalizable text from C++ source code.*/
    class cpp_i18n_review : public i18n_review
        {
    public:
        /** @brief Main interface for extracting resource text from C++ source code.
            @param cpp_text The C++ code text to extract text from.
            @param file_name The (optional) name of source file being analyzed.*/
        void operator()(const std::wstring_view cpp_text,
                        const std::wstring& file_name = L"") final;
    private:
#ifdef __UNITTEST
    public:
#endif
        /// @brief Strips off the trailing template and global accessor (i.e., "::")
        ///     information from a function/variable.
        void remove_decorations(std::wstring& str) const final;
        /// @returns @c true if provided variable type is just a decorator after the real
        ///     variable type (e.g., const) and should be skipped.
        /// @param variableType The parsed variable type to review.
        bool is_variable_type_decorator(const std::wstring_view variableType) const final
            { return variableType.compare(L"const") == 0; }
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
        wchar_t* skip_preprocessor_define_block(wchar_t* directiveStart);

        /// @brief Parses and processes an inline assembly block.
        /// @param asmStart The start of the asm section.
        /// @returns The end of the current asm block.
        [[nodiscard]]
        wchar_t* process_assembly_block(wchar_t* asmStart);

        /// @returns @c true if text is an inline assembly block.
        [[nodiscard]]
        bool is_assembly_block(const wchar_t* text) const noexcept
            {
            assert(text);
            if (!text)
                { return false; }
            return ((std::wcsncmp(text, L"asm", 3) == 0 &&
                    std::iswspace(text[3])) ||
                    (std::wcsncmp(text, L"__asm__", 7) == 0 &&
                    (std::iswspace(text[7]) || text[7] == L'(')) ||
                    (std::wcsncmp(text, L"__asm", 5) == 0 &&
                    std::iswspace(text[5])));
            }
        };
    }

/** @}*/

#endif //__CPP_I18N_REVIEW_H__
