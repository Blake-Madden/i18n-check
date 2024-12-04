/********************************************************************************
 * Copyright (c) 2021-2024 Blake Madden
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

#ifndef __I18N_INPUTS_H__
#define __I18N_INPUTS_H__

#include <filesystem>
#include <string>
#include <vector>

namespace i18n_check
    {
    struct excluded_results
        {
        std::vector<std::filesystem::path> m_excludedPaths;
        std::vector<std::filesystem::path> m_excludedFiles;
        };

    /** @brief Returns a list of excluded folders and files from a list of patterns.
        @param inputFolder The root folder being analyzed.
        @param excluded The list of excluded files and folder.
            This can be full paths, relative paths, folder names, filenames, etc.
        @returns The fully-expanded paths for folders and files to exclude.*/
    [[nodiscard]]
    excluded_results get_paths_files_to_exclude(const std::filesystem::path& inputFolder,
                                                const std::vector<std::filesystem::path>& excluded);

    /** @brief Retrieves a list of files to analyze from a root folder, excluding any files and
       paths that were requested to be ignored.
        @param inputFolder The root folder to scan.
        @param excludedPaths Folders that should be ignored.
            These should be full paths, not folder names.
        @param excludedFiles Files that should be ignored. These should be full paths, not
       filenames.
        @returns A list of files that can be analyzed.*/
    [[nodiscard]]
    std::vector<std::filesystem::path>
    get_files_to_analyze(const std::filesystem::path& inputFolder,
                         const std::vector<std::filesystem::path>& excludedPaths,
                         const std::vector<std::filesystem::path>& excludedFiles);
    } // namespace i18n_check

/** @}*/

#endif //__I18N_INPUTS_H__
