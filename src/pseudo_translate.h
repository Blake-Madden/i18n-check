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
#include <map>
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace i18n_check
    {
    /// @brief The different ways a message can be pseudo translated.
    enum class pseudo_translation_method
        {
        /// @brief Uppercase all letters.
        all_caps
        };

    /** @brief Class to pseudo translate a file.*/
    class pseudo_translater
        {
      public:
        /** @brief Pseudo translates a gettext PO file.
            @param[in,out] poFileText The PO file's text to translate.*/
        void po_file(std::wstring& poFileText) const;

      private:
        static size_t find_po_msg_end(const std::wstring& poFileText, const size_t startPos);
        static std::wstring mutate_message_caps(const std::wstring& msg);
        pseudo_translation_method m_transType{ pseudo_translation_method::all_caps };
        };
    } // namespace i18n_check

/** @}*/

#endif // __PSEUDO_FILE_REVIEW_H__
