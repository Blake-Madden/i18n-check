/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
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

        /** @brief Increases the width of each pseudo-translation.
            @param increasedWidth The percent to increase the widths of the pseudo-translation\n
                (should be between 0-100).*/
        void increase_width(const uint8_t increasedWidth) noexcept
            {
            m_width_increase = std::clamp<uint8_t>(increasedWidth, 0, 100);
            }

        /// @brief Adds a unique ID in front of every pseudo-tranlated string.
        /// @param enable @c true to enable tracking.
        void enable_tracking(const bool enable) noexcept { m_track = enable; }

        /// @brief If tracking is enabled, then resets the ID incrementer to zero.
        void reset_tracking() noexcept { m_current_id = 0; }

      private:
        static const std::map<wchar_t, wchar_t> m_euro_char_map;
        static size_t find_po_msg_end(const std::wstring& poFileText, const size_t startPos);
        std::wstring mutate_message(const std::wstring& msg) const;
        pseudo_translation_method m_trans_type{ pseudo_translation_method::all_caps };
        bool m_add_surrounding_brackets{ false };
        uint8_t m_width_increase{ 0 };
        bool m_track{ false };
        mutable int64_t m_current_id{ 0 };
        };
    } // namespace i18n_check

/** @}*/

#endif // __PSEUDO_FILE_REVIEW_H__
