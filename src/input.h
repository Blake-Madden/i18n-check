/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_INPUTS_H__
#define __I18N_INPUTS_H__

#include <filesystem>
#include <string>
#include <vector>

namespace i18n_check
    {
    struct excluded_results
        {
        std::vector<std::wstring> excludedPaths;
        std::vector<std::wstring> excludedFiles;
        };

    /** @brief Returns a list of excluded folders and files from a list of patterns.
        @param inputFolder The root folder being analyzed.
        @param excluded The list of excluded files and folder.
            This can be full paths, relative paths, folder names, filenames, etc.
        @returns The fully-expanded paths for folders and files to exclude.*/
    [[nodiscard]]
    excluded_results get_paths_files_to_exclude(const std::filesystem::path& inputFolder,
                                                const std::vector<std::wstring>& excluded);

    /** @brief Retrieves a list of files to analyze from a root folder, excluding any files and
       paths that were requested to be ignored.
        @param inputFolder The root folder to scan.
        @param excludedPaths Folders that should be ignored.
            These should be full paths, not folder names.
        @param excludedFiles Files that should be ignored. These should be full paths, not
       filenames.
        @returns A list of files that can be analyzed.*/
    [[nodiscard]]
    std::vector<std::wstring> get_files_to_analyze(const std::filesystem::path& inputFolder,
                                                  const std::vector<std::wstring>& excludedPaths,
                                                  const std::vector<std::wstring>& excludedFiles);
    } // namespace i18n_check

/** @}*/

#endif //__I18N_INPUTS_H__
