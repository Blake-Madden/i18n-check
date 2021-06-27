/** @addtogroup Internationalization
    @brief Classes to extract resources from source files.
    @date 2021
    @copyright Oleander Software, Ltd.
    @author Oleander Software, Ltd.
    @details This program is proprietary and not licensable.
* @{*/

#ifndef __CPP_I18N_REVIEW_H__
#define __CPP_I18N_REVIEW_H__

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include "i18n_review.h"

/// Classes for checking source code for internationalization/localization issues.
namespace i18n_check
    {
    /** @brief Class to extract and review localizable/nonlocalizable text from C++ source code.*/
    class cpp_i18n_review : public i18n_review
        {
    public:
        /** @brief Main interface for extracting resource text from C++ source code.
            @param cpp_text The C++ code text to extract text from.
            @param text_length The length of the text.
            @param file_name The (optional) name of source file being analyzed.*/
        void operator()(const wchar_t* cpp_text, const size_t text_length, const std::wstring& file_name = L"") final;
    private:
#ifdef __UNITTEST
    public:
#endif
        /// Strips off the trailing template and global accessor (i.e., "::") information from a function/variable.
        void remove_decorations(std::wstring& str) const final;
        /// Parses and processes a preprocessor directive.
        /// @param directiveStart The start of the preprocessor section.
        /// @param directivePos The position in the overall text that the preprocessor block is at.
        /// @returns The end of the current preprocessor block.
        [[nodiscard]] wchar_t* process_preprocessor_directive(wchar_t* directiveStart, const size_t directivePos);

        /// Parses and processes an inline assembly block.
        /// @param asmStart The start of the asm section.
        /// @returns The end of the current asm block.
        [[nodiscard]] wchar_t* process_assembly_block(wchar_t* asmStart);

        /// @returns True if text is an inline assembly block.
        [[nodiscard]] bool is_assembly_block(const wchar_t* text) const noexcept
            {
            assert(text);
            if (!text)
                { return false; }
            return ((std::wcsncmp(text, L"asm", 3) == 0 &&
                    std::iswspace(text[3])) ||
                    (std::wcsncmp(text, L"__asm__", 7) == 0 &&
                    std::iswspace(text[7])) ||
                    (std::wcsncmp(text, L"__asm", 5) == 0 &&
                    std::iswspace(text[5])));
            }
        };
    }

/** @}*/

#endif //__CPP_I18N_REVIEW_H__
