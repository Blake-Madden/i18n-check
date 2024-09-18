/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
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
        /// @returns All loaded catalog entries (and any detected errors connected to them).\n
        ///     The first key is the filepath, and the second is all catalog entries in that file.
        [[nodiscard]]
        const std::vector<std::pair<std::wstring, translation_catalog_entry>>&
        get_catalog_entries() const noexcept
            {
            return m_catalog_entries;
            }

        /// @returns All loaded catalog entries (and any detected errors connected to them).\n
        ///     The first key is the filepath, and the second is all catalog entries in that file.
        [[nodiscard]]
        std::vector<std::pair<std::wstring, translation_catalog_entry>>&
        get_catalog_entries() noexcept
            {
            return m_catalog_entries;
            }

        /// @brief Reviews the loaded translation catalog entries for issues.
        void review_strings() override;

      private:
        void operator()([[maybe_unused]] std::wstring_view,
                        [[maybe_unused]] const std::wstring&) override
            {
            }

        std::vector<std::pair<std::wstring, translation_catalog_entry>> m_catalog_entries;
        };
    } // namespace i18n_check

/** @}*/

#endif // __TRANS_CAT_FILE_REVIEW_H__
