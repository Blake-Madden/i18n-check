/********************************************************************************
 * Copyright (c) 2023-2024 Blake Madden
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

#ifndef __RC_FILE_REVIEW_H__
#define __RC_FILE_REVIEW_H__

#include "i18n_review.h"
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

/// @brief Class for reviewing a Microsoft Windows resource file (*.RC).
namespace i18n_check
    {
    /** @brief Class to extract and review localizable/nonlocalizable text from C++ source code.*/
    class rc_file_review : public i18n_review
        {
      public:
        /// @brief Constructor.
        /// @param verbose @c true to include verbose warnings.
        explicit rc_file_review(const bool verbose) : i18n_review(verbose) {}

        /** @brief Main interface for extracting resource text from C++ source code.
            @param rcFileText The RC file's text to analyze.
            @param fileName The (optional) name of source file being analyzed.*/
        void operator()(const std::wstring_view rcFileText,
                        const std::filesystem::path& fileName) final;

        /// @returns Font sizes used for dialogs that are too large or small.
        [[nodiscard]]
        const std::vector<string_info>& get_bad_dialog_font_sizes() const noexcept
            {
            return m_badFontSizes;
            }

        /// @returns Fonts used for dialogs that aren't MS Shell Dlg variants.
        [[nodiscard]]
        const std::vector<string_info>& get_non_system_dialog_fonts() const noexcept
            {
            return m_nonSystemFontNames;
            }

      private:
        std::vector<string_info> m_badFontSizes;
        std::vector<string_info> m_nonSystemFontNames;
        };
    } // namespace i18n_check

/** @}*/

#endif // __RC_FILE_REVIEW_H__
