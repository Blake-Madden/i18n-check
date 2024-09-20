/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_ANALYZE_H__
#define __I18N_ANALYZE_H__

#include "cpp_i18n_review.h"
#include "i18n_string_util.h"
#include "po_file_review.h"
#include "pseudo_translate.h"
#include "rc_file_review.h"
#include "unicode_extract_text.h"
#include "utfcpp/source/utf8.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace i18n_check
    {
    /// @brief Progress callback for analyze().
    using analyze_callback = std::function<bool(const size_t, const size_t, const std::wstring&)>;

    /// @private
    std::pair<bool, std::wstring> read_utf16_file(const std::filesystem::path& filePath);
    /// @private
    bool valid_utf8_file(const std::filesystem::path& filePath, bool& startsWithBom);
    /// @private
    std::pair<bool, std::wstring> read_utf8_file(const std::filesystem::path& filePath,
                                                 bool& startsWithBom);

    /** @brief Runs all analyzers on a set of files.
        @param filesToAnalyze The files to analyze.
        @param[in,out] cpp The C++ analyzer that was used.
        @param[in,out] rc The Windows RC file analyzer that was used.
        @param[in,out] po The PO file analyzer that was used.
        @param[out] filesThatShouldBeConvertedToUTF8 Files that should be converted to UTF-8.
        @param[out] filesThatContainUTF8Signature UTF-8 files that contain a
            Windows UTF-8 file signature.
        @param callback Callback function to display the progress.
            Takes the current file index, overall file count, and the name of the current file.
            Returning @c false indicates that the user cancelled the analysis.*/
    void analyze(const std::vector<std::wstring>& filesToAnalyze, i18n_check::cpp_i18n_review& cpp,
                 i18n_check::rc_file_review& rc, i18n_check::po_file_review& po,
                 std::vector<std::wstring>& filesThatShouldBeConvertedToUTF8,
                 std::vector<std::wstring>& filesThatContainUTF8Signature,
                 analyze_callback callback);

    /** @brief Pseudo translates a set of files.
        @details Copies of each file are made in the same folder with
            'pseudo_' prepended to the file name.
        @param filesToTranslate The files to translate.
        @param pseudoMethod How to pseudo-translate the content.
        @param widthIncrease How much width to increase the pseudo-translation from
            the source string. This will pad the string with hyphens.
        @param addSurroundingBrackets @c true to add square brackets and bangs
            around each translation.
        @param callback Callback function to display the progress.
            Takes the current file index, overall file count, and the name of the current file.
            Returning @c false indicates that the user cancelled the analysis.*/
    void pseudo_translate(const std::vector<std::wstring>& filesToTranslate,
                          i18n_check::pseudo_translation_method pseudoMethod,
                          bool addSurroundingBrackets, uint8_t widthIncrease,
                          analyze_callback callback);

    /** @returns A formatted summary of the results.
        @param[in,out] cpp The C++ analyzer that was used.
        @param[in,out] rc The Windows RC file analyzer that was used.
        @param[in,out] po The PO file analyzer that was used.
        @param[out] filesThatShouldBeConvertedToUTF8 Files that should be converted to UTF-8.
        @param[out] filesThatContainUTF8Signature UTF-8 files that contain a
            Windows UTF-8 file signature.
        @param verbose @c true to include debug output.*/
    [[nodiscard]]
    std::wstringstream format_results(i18n_check::cpp_i18n_review& cpp,
                                      i18n_check::rc_file_review& rc,
                                      i18n_check::po_file_review& po,
                                      std::vector<std::wstring>& filesThatShouldBeConvertedToUTF8,
                                      std::vector<std::wstring>& filesThatContainUTF8Signature,
                                      const bool verbose = false);

    /** @returns A formatted summary of the options used.
        @param cpp The C++ analyzer that was used.
        @param rc The Windows RC file analyzer that was used.
        @param po The PO file analyzer that was used.*/
    [[nodiscard]]
    std::wstringstream format_summary(const i18n_check::cpp_i18n_review& cpp,
                                      const i18n_check::rc_file_review& rc,
                                      const i18n_check::po_file_review& po);

    /// @brief Gets the file type of a file based on extension.
    /// @param file The file path.
    /// @returns The file type, based on extension.
    [[nodiscard]]
    inline file_review_type get_file_type(const std::wstring& file)
        {
        const auto ext = std::filesystem::path{ file }.extension();
        if (ext.compare(std::filesystem::path(L".rc")) == 0)
            {
            return file_review_type::rc;
            }
        else if (ext.compare(std::filesystem::path(L".po")) == 0 ||
                 ext.compare(std::filesystem::path(L".pot")) == 0)
            {
            return file_review_type::po;
            }
        else
            {
            return file_review_type::cpp;
            }
        }
    } // namespace i18n_check

/** @}*/

#endif //__I18N_ANALYZE_H__
