/********************************************************************************
 * Copyright (c) 2021-2025 Blake Madden
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

#ifndef __I18N_ANALYZE_H__
#define __I18N_ANALYZE_H__

#include "cpp_i18n_review.h"
#include "csharp_i18n_review.h"
#include "i18n_string_util.h"
#include "po_file_review.h"
#include "pseudo_translate.h"
#include "rc_file_review.h"
#include "unicode_extract_text.h"
#include "utfcpp/source/utf8.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace i18n_check
    {
    /// @private
    std::pair<bool, std::wstring> read_utf16_file(const std::filesystem::path& filePath);
    /// @private
    bool valid_utf8_file(const std::filesystem::path& filePath, bool& startsWithBom);
    /// @private
    std::pair<bool, std::wstring> read_utf8_file(const std::filesystem::path& filePath,
                                                 bool& startsWithBom);

    /// @brief Gets the file type of a file based on extension.
    /// @param file The file path.
    /// @returns The file type, based on extension.
    [[nodiscard]]
    inline file_review_type get_file_type(const std::filesystem::path& file)
        {
        if (file.extension().compare(std::filesystem::path(L".rc")) == 0)
            {
            return file_review_type::rc;
            }
        else if (file.extension().compare(std::filesystem::path(L".po")) == 0 ||
                 file.extension().compare(std::filesystem::path(L".pot")) == 0)
            {
            return file_review_type::po;
            }
        else if (file.extension().compare(std::filesystem::path(L".cs")) == 0)
            {
            return file_review_type::cs;
            }
        else if (file.extension().compare(std::filesystem::path(L".cpp")) == 0 ||
                 file.extension().compare(std::filesystem::path(L".c")) == 0 ||
                 file.extension().compare(std::filesystem::path(L".hpp")) == 0 ||
                 file.extension().compare(std::filesystem::path(L".h")) == 0)
            {
            return file_review_type::cpp;
            }
        else
            {
            return file_review_type::unknown;
            }
        }

    /// @brief Determines if a file is a source file, based on extension.
    /// @param file The file path.
    /// @returns @c true if a source file.
    [[nodiscard]]
    inline bool is_source_file(const std::filesystem::path& file)
        {
        const auto fileType{ get_file_type(file) };
        return (fileType == file_review_type::cpp || fileType == file_review_type::cs);
        }

    /// @brief Wrapper class to analyze and summarize a batch of files.
    class batch_analyze
        {
      public:
        /** @brief Constructor which loads the sub-analyzers to use against a batch of files.
            @param[in,out] cpp The C++ analyzer that was used.
            @param[in,out] rc The Windows RC file analyzer that was used.
            @param[in,out] po The PO file analyzer that was used.
            @param[in,out] csharp The C# file analyzer that was used.*/
        batch_analyze(i18n_check::cpp_i18n_review* cpp, i18n_check::rc_file_review* rc,
                      i18n_check::po_file_review* po, i18n_check::cpp_i18n_review* csharp)
            : m_cpp(cpp), m_rc(rc), m_po(po), m_csharp(csharp)
            {
            }

        /// @private
        batch_analyze(const batch_analyze&) = delete;
        /// @private
        batch_analyze& operator=(const batch_analyze&) = delete;

        /** @brief Runs all analyzers on a set of files.
            @param filesToAnalyze The files to analyze.
            @param resetCallback Callback function to tell the progress system in @c callback
                how many items to expect to be processed.
            @param callback Callback function to display the progress.
                Takes the current file index, overall file count, and the name of the current file.
                Returning @c false indicates that the user cancelled the analysis.*/
        void analyze(const std::vector<std::filesystem::path>& filesToAnalyze,
                     analyze_callback_reset resetCallback, analyze_callback callback);

        /** @brief Pseudo translates a set of files.
            @details Copies of each file are made in the same folder with
                'pseudo_' prepended to the file name.
            @param filesToTranslate The files to translate.
            @param pseudoMethod How to pseudo-translate the content.
            @param widthChange How much width to increase or decrease the pseudo-translation from
                the source string. This will pad the string with hyphens.
            @param addTrackingIds @c true to add unique IDs in front the the strings.
            @param addSurroundingBrackets @c true to add square brackets and bangs
                around each translation.
            @param resetCallback Callback function to tell the progress system in @c callback
                how many items to expect to be processed.
            @param callback Callback function to display the progress.
                Takes the current file index, overall file count, and the name of the current file.
                Returning @c false indicates that the user cancelled the analysis.*/
        void pseudo_translate(const std::vector<std::filesystem::path>& filesToTranslate,
                              i18n_check::pseudo_translation_method pseudoMethod,
                              bool addSurroundingBrackets, int8_t widthChange, bool addTrackingIds,
                              analyze_callback_reset resetCallback, analyze_callback callback);

        /** @returns A formatted summary of the results.
            @param verbose @c true to include debug output.*/
        [[nodiscard]]
        std::wstringstream format_results(const bool verbose = false);

        /** @returns A formatted summary of the options used.
            @param verbose Whether to include information about which checks
                were performed in the summary.*/
        [[nodiscard]]
        std::wstringstream format_summary(const bool verbose = false);

        /// @returns The files that should be converted to UTF-8 (from the last call to analyze()).
        [[nodiscard]]
        std::vector<std::filesystem::path>& get_files_that_should_be_converted_to_utf() noexcept
            {
            return m_filesThatShouldBeConvertedToUTF8;
            }

        /// @returns The files that contain UTF-8 signatures (from the last call to analyze()).
        [[nodiscard]]
        std::vector<std::filesystem::path>& get_files_that_contain_utf_signatures() noexcept
            {
            return m_filesThatContainUTF8Signature;
            }

        /// @returns Messages logged during previous analyzes and pseudo-translations.
        [[nodiscard]]
        std::wstring& get_log_report() noexcept
            {
            return m_logReport;
            }

      private:
        i18n_check::cpp_i18n_review* m_cpp{ nullptr };
        i18n_check::rc_file_review* m_rc{ nullptr };
        i18n_check::po_file_review* m_po{ nullptr };
        i18n_check::cpp_i18n_review* m_csharp{ nullptr };

        std::vector<std::filesystem::path> m_filesThatShouldBeConvertedToUTF8;
        std::vector<std::filesystem::path> m_filesThatContainUTF8Signature;

        std::wstring m_logReport;
        };
    } // namespace i18n_check

/** @}*/

#endif //__I18N_ANALYZE_H__
