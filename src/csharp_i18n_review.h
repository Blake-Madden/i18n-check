/********************************************************************************
 * Copyright (c) 2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef __CSHARP_I18N_REVIEW_H__
#define __CSHARP_I18N_REVIEW_H__

#include "cpp_i18n_review.h"

/// @brief Classes for checking source code for internationalization/localization issues.
namespace i18n_check
    {
    /** @brief Class to extract and review localizable/nonlocalizable text from C# source code.*/
    class csharp_i18n_review : public cpp_i18n_review
        {
      public:
        /// @brief Constructor.
        /// @param verbose @c true to include verbose warnings.
        csharp_i18n_review(const bool verbose) : cpp_i18n_review(verbose)
            {
            m_collapse_double_quotes = true;
            }

      private:
        void remove_decorations(std::wstring& str) const final;

        /// @returns The end of a raw string.
        /// @param text The current string to parse.
        /// @param chr The tag indicating what type of raw string this is.
        [[nodiscard]]
        wchar_t* find_raw_string_end(wchar_t* text, [[maybe_unused]] const wchar_t chr) const final;

        /// @returns How many characters a to step into/out of a raw string.
        ///     In other words, how many characters are inside of the string marking the
        ///     sentinel boundaries of the raw content.
        /// @param chr The tag indicating what type of raw string this is.
        [[nodiscard]]
        size_t get_raw_step_size(const wchar_t chr) const final
            {
            return (chr == L'\"') ? 2 : 0;
            }

        /// @return @c true if a character indicates a raw string.
        /// @param chr The character in front of the string.
        [[nodiscard]]
        bool is_raw_string_marker(const wchar_t chr) const final
            {
            return chr == L'@';
            }
        };
    } // namespace i18n_check

/** @}*/

#endif //__CSHARP_I18N_REVIEW_H__
