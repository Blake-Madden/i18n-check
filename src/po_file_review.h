/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __PO_FILE_REVIEW_H__
#define __PO_FILE_REVIEW_H__

#include "i18n_review.h"
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace i18n_check
    {
    /// @brief Types of printf languages that a PO file can contain.
    /// @details This determines which type of syntax is expected.
    enum class po_format_string
        {
        /// @brief This string is not meant for `printf()`-like functions.
        no_format,
        /// @brief C/C++ style `printf()`.
        cpp_format,
        /// @brief Objective-C.
        objc_format
        };

    /// @brief An entry in a PO file, which contains source and translation strings,
    ///     printf syntax that they use, and any encountered issues.
    /// @details `printf` syntax:
    ///
    /// https://en.cppreference.com/w/c/io/fprintf
    ///
    /// `printf` positional arguments:
    ///
    /// https://learn.microsoft.com/en-us/cpp/c-runtime-library/printf-p-positional-parameters?view=msvc-170
    struct po_catalog_entry
        {
        /// @brief The main source string.
        std::wstring m_source;
        /// @brief An optional plural variation of the source string.
        std::wstring m_source_plural;
        /// @brief The translation of the main source string.
        std::wstring m_translation;
        /// @brief The translation of the optional plural form of the source string.
        std::wstring m_translation_plural;
        /// @brief The printf syntax used by the strings.
        po_format_string m_po_format{ po_format_string::no_format };
        /// @brief Any issues detected in this entry.
        std::vector<std::pair<translation_issue, std::wstring>> m_issues;
        /// @brief The position in the file.
        size_t m_line{ std::wstring_view::npos };
        };

    /** @brief Class to extract and review translations from PO catalogs.*/
    class po_file_review : public i18n_review
        {
      public:
        /** @brief Main interface for extracting resource text from C++ source code.
            @param poFileText The PO file's text to analyze.
            @param fileName The (optional) name of source file being analyzed.*/
        void operator()(std::wstring_view poFileText, const std::wstring& fileName) final;

        /// @returns All loaded catalog entries (and any detected errors connected to them).\n
        ///     The first key is the filepath, and the second is all catalog entries in that file.
        [[nodiscard]]
        const std::vector<std::pair<std::wstring, po_catalog_entry>>&
        get_catalog_entries() const noexcept
            {
            return m_catalog_entries;
            }

        /// @brief Reviews the loaded PO content for issues.
        void review_strings() final
            {
            if (static_cast<bool>(m_reviewStyles & check_mismatching_printf_commands))
                {
                review_prinf_issues();
                }
            }

        /// @brief Whether fuzzy translations should be reviewed.
        /// @param enable @c true to review fuzzy translation entries; @c false to ignore them.
        /// @note It is recommended that this be false, as fuzzy translations are generally
        ///     considered to be "needing work" and are known to have issues.
        void review_fuzzy_translations(const bool enable) noexcept { m_reviewFuzzy = enable; }

        /// @returns @c true if fuzzy translations are being reviewed.\n
        ///     The default is @c false.
        [[nodiscard]]
        bool is_reviewing_fuzzy_translations() const noexcept
            {
            return m_reviewFuzzy;
            }

      private:
        [[nodiscard]]
        static std::tuple<bool, std::wstring_view, size_t>
        read_catalog_entry(std::wstring_view& poFileText);

        [[nodiscard]]
        static std::pair<bool, std::wstring> read_msg(std::wstring_view& poFileText,
                                               const std::wstring_view msgTag);

        void review_prinf_issues();

        [[nodiscard]]
        static std::vector<std::wstring>
        convert_positional_cpp_printf(const std::vector<std::wstring>& printfCommands,
                                      std::wstring& errorInfo);

        [[nodiscard]]
        static std::vector<std::wstring> load_cpp_printf_commands(const std::wstring& resource,
                                                                  std::wstring& errorInfo);

        std::vector<std::pair<std::wstring, po_catalog_entry>> m_catalog_entries;

        bool m_reviewFuzzy{ false };
        };
    } // namespace i18n_check

/** @}*/

#endif // __PO_FILE_REVIEW_H__
