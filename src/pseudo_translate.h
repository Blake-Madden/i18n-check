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

#ifndef __PSEUDO_FILE_REVIEW_H__
#define __PSEUDO_FILE_REVIEW_H__

#include "i18n_review.h"
#include <cmath>
#ifndef CPPCHECK_SKIP_SECTION
    #if __has_include(<format>)
        #include <format>
    #endif
#endif
#include <map>
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace i18n_check
    {
    /// @brief The different ways a message can be pseudo-translated.
    enum class pseudo_translation_method
        {
        /// @brief Do not pseudo-translate anything.
        none,
        /// @brief Uppercase all letters.
        all_caps,
        /// @brief European (accented) characters.
        european_characters,
        /// @brief Fills with 'X'es
        Xx_es
        };

    /** @brief Class to pseudo-translate a file.*/
    class pseudo_translater
        {
      public:
        /** @brief Pseudo-translates a gettext PO file.
            @param[in,out] poFileText The PO file's text to translate.
            @todo Add support for ignoring C++20 format library specifiers.*/
        void translate_po_file(std::wstring& poFileText) const;

        /** @brief Sets how to pseudo-translate the strings.
            @param method The pseudo-translation method.*/
        void set_pseudo_method(const pseudo_translation_method method) noexcept
            {
            m_trans_type = method;
            }

        /** @brief Whether to add surrounding square brackets and bangs around translations.
            @param addBrackets @c true to add the brackets.*/
        void add_surrounding_brackets(const bool addBrackets) noexcept
            {
            m_add_surrounding_brackets = addBrackets;
            }

        /** @brief Increases or decreases the width of each pseudo-translation.
            @param changedWidth The percent to increase or decrease the widths of the\n
                pseudo-translation (should be between -50 and 100).*/
        void change_width(const int8_t changedWidth) noexcept
            {
            m_width_change = std::clamp<int8_t>(changedWidth, -50, 100);
            }

        /// @brief Adds a unique ID in front of every pseudo-tranlated string.
        /// @param enable @c true to enable tracking.
        void enable_tracking(const bool enable) noexcept { m_track = enable; }

        /// @brief If tracking is enabled, then resets the ID incrementer to zero.
        void reset_tracking() noexcept { m_current_id = 0; }

        /// @brief Psuudo-translates @c msg using the current settings.
        /// @param msg The string to pseudo-translate.
        /// @returns The pseudo-translated string.
        std::wstring mutate_message(const std::wstring& msg) const;

      private:
        static const std::map<wchar_t, wchar_t> m_euro_char_map;
        pseudo_translation_method m_trans_type{ pseudo_translation_method::all_caps };
        bool m_add_surrounding_brackets{ false };
        int8_t m_width_change{ 40 };
        bool m_track{ false };
        mutable int64_t m_current_id{ 0 };
        };
    } // namespace i18n_check

/** @}*/

#endif // __PSEUDO_FILE_REVIEW_H__
