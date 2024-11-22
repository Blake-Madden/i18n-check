/********************************************************************************
 * Copyright (c) 2021-2024 Blake Madden
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

#include "input.h"
#include "i18n_review.h"

namespace i18n_check
    {
    //------------------------------------------------------
    excluded_results get_paths_files_to_exclude(const std::filesystem::path& inputFolder,
                                                const std::vector<std::filesystem::path>& excluded)
        {
        excluded_results excResults;

        for (const auto& excItem : excluded)
            {
            if (excItem.empty())
                {
                continue;
                }
            if (std::filesystem::exists(excItem))
                {
                if (std::filesystem::is_directory(excItem))
                    {
                    excResults.m_excludedPaths.push_back(excItem);
                    const auto folderToRecurse{ excResults.m_excludedPaths.back() };
                    // add subdirectories
                    for (const auto& p :
                         std::filesystem::recursive_directory_iterator(folderToRecurse))
                        {
                        if (std::filesystem::exists(p) && p.is_directory())
                            {
                            excResults.m_excludedPaths.push_back(p.path());
                            }
                        }
                    }
                else
                    {
                    excResults.m_excludedFiles.push_back(excItem);
                    }
                }
            // if not a full path, just a subdirectory path
            else if (const auto relPath = std::filesystem::path{ inputFolder } / excItem;
                     std::filesystem::exists(relPath))
                {
                if (std::filesystem::is_directory(relPath))
                    {
                    excResults.m_excludedPaths.push_back(relPath);
                    const auto folderToRecurse{ excResults.m_excludedPaths.back() };
                    // add subdirectories
                    for (const auto& p :
                         std::filesystem::recursive_directory_iterator(folderToRecurse))
                        {
                        if (std::filesystem::exists(p) && p.is_directory())
                            {
                            excResults.m_excludedPaths.push_back(p.path());
                            }
                        }
                    }
                else
                    {
                    excResults.m_excludedFiles.push_back(relPath);
                    }
                }
            else
                {
                continue;
                }
            }

        return excResults;
        }

    //------------------------------------------------------
    std::vector<std::filesystem::path>
    get_files_to_analyze(const std::filesystem::path& inputFolder,
                         const std::vector<std::filesystem::path>& excludedPaths,
                         const std::vector<std::filesystem::path>& excludedFiles)
        {
        std::vector<std::filesystem::path> filesToAnalyze;

        if (std::filesystem::is_regular_file(inputFolder) && std::filesystem::exists(inputFolder))
            {
            filesToAnalyze.push_back(inputFolder);
            }
        else if (std::filesystem::is_directory(inputFolder) && std::filesystem::exists(inputFolder))
            {
            for (const auto& p : std::filesystem::recursive_directory_iterator(inputFolder))
                {
                const auto ext = p.path().extension();
                bool inExcludedPath{ false };
                for (const auto& ePath : excludedPaths)
                    {
                    std::filesystem::path excPath(ePath,
                                                  std::filesystem::path::format::native_format);
                    if (p.exists() && std::filesystem::exists(excPath) &&
                        std::filesystem::equivalent(p.path().parent_path(), excPath))
                        {
                        inExcludedPath = true;
                        break;
                        }
                    }
                // compare against excluded files if not already in an excluded folder
                if (!inExcludedPath)
                    {
                    for (const auto& eFile : excludedFiles)
                        {
                        std::filesystem::path excFile(eFile,
                                                      std::filesystem::path::format::native_format);
                        if (p.exists() && std::filesystem::exists(excFile) &&
                            std::filesystem::equivalent(p, excFile))
                            {
                            inExcludedPath = true;
                            break;
                            }
                        }
                    }
                if (p.exists() && p.is_regular_file() && !inExcludedPath &&
                    (ext.compare(std::filesystem::path(L".rc")) == 0 ||
                     ext.compare(std::filesystem::path(L".c")) == 0 ||
                     ext.compare(std::filesystem::path(L".cs")) == 0 ||
                     ext.compare(std::filesystem::path(L".cpp")) == 0 ||
                     ext.compare(std::filesystem::path(L".h")) == 0 ||
                     ext.compare(std::filesystem::path(L".hpp")) == 0 ||
                     ext.compare(std::filesystem::path(L".po")) == 0 ||
                     ext.compare(std::filesystem::path(L".pot")) == 0) &&
                    // ignore CMake build files
                    p.path().filename().compare(L"CMakeCXXCompilerId.cpp") != 0 &&
                    p.path().filename().compare(L"CMakeCCompilerId.c") != 0 &&
                    // main catch2 files
                    p.path().filename().compare(L"catch.hpp") != 0 &&
                    p.path().filename().compare(L"catch_amalgamated.cpp") != 0 &&
                    p.path().filename().compare(L"catch_amalgamated.hpp") != 0 &&
                // ignore pseudo-translated message catalogs what we previously generated
#if CHECK_GCC_VERSION(12, 2, 1)
                    !p.path().filename().wstring().starts_with(L"pseudo_"))
#else
                    !p.path().filename().string().starts_with("pseudo_"))
#endif
                    {
                    filesToAnalyze.push_back(p.path());
                    }
                }
            }

        return filesToAnalyze;
        }
    } // namespace i18n_check
