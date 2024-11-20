/********************************************************************************
 * Copyright (c) 2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef __TRANS_CAT_FILE_REVIEW_H__
#define __TRANS_CAT_FILE_REVIEW_H__

#include "i18n_review.h"
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace i18n_check
    {
    /** @brief Class to extract and review translations from translation catalogs.*/
    class translation_catalog_review : public i18n_review
        {
      public:
        /// @brief Constructor.
        /// @param verbose @c true to include verbose warnings.
        translation_catalog_review(const bool verbose) : i18n_review(verbose) {}

        /// @returns All loaded catalog entries (and any detected errors connected to them).\n
        ///     The first key is the filepath, and the second is all catalog entries in that file.
        [[nodiscard]]
        const std::vector<std::pair<std::filesystem::path, translation_catalog_entry>>&
        get_catalog_entries() const noexcept
            {
            return m_catalog_entries;
            }

        /// @returns All loaded catalog entries (and any detected errors connected to them).\n
        ///     The first key is the filepath, and the second is all catalog entries in that file.
        [[nodiscard]]
        std::vector<std::pair<std::filesystem::path, translation_catalog_entry>>&
        get_catalog_entries() noexcept
            {
            return m_catalog_entries;
            }

        /** @brief Reviews the loaded translation catalog entries for issues.
            @param resetCallback Callback function to tell the progress system in @c callback
                how many items to expect to be processed.
            @param callback Callback function to display the progress.
                Takes the current file index, overall file count, and the name of the current file.
                Returning @c false indicates that the user cancelled the analysis.*/
        void review_strings(analyze_callback_reset resetCallback,
                            analyze_callback callback) override;

      private:
        void operator()([[maybe_unused]] std::wstring_view,
                        [[maybe_unused]] const std::filesystem::path&) override
            {
            }

        std::vector<std::pair<std::filesystem::path, translation_catalog_entry>> m_catalog_entries;
        };
    } // namespace i18n_check

/** @}*/

#endif // __TRANS_CAT_FILE_REVIEW_H__
