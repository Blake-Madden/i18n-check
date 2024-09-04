//////////////////////////////////////////////////////////////////////////////
// Name:        input.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "input.h"

namespace i18n_check
    {
    //------------------------------------------------------
    excluded_results get_paths_files_to_exclude(const std::filesystem::path& inputFolder,
                                                const std::vector<std::wstring>& excluded)
        {
        excluded_results excResults;

        for (const auto& excItem : excluded)
            {
            if (std::filesystem::exists(excItem))
                {
                if (std::filesystem::is_directory(excItem))
                    {
                    excResults.excludedPaths.push_back(excItem);
                    const auto folderToRecurse{ excResults.excludedPaths.back() };
                    // add subdirectories
                    for (const auto& p :
                         std::filesystem::recursive_directory_iterator(folderToRecurse))
                        {
                        if (std::filesystem::exists(p) && p.is_directory())
                            {
                            excResults.excludedPaths.push_back(p.path().wstring());
                            }
                        }
                    }
                else
                    {
                    excResults.excludedFiles.push_back(excItem);
                    }
                }
            // if not a full path, just a subdirectory path
            else if (const auto relPath = std::filesystem::path{ inputFolder } / excItem;
                     std::filesystem::exists(relPath))
                {
                if (std::filesystem::is_directory(relPath))
                    {
                    excResults.excludedPaths.push_back(relPath.wstring());
                    const auto folderToRecurse{ excResults.excludedPaths.back() };
                    // add subdirectories
                    for (const auto& p :
                         std::filesystem::recursive_directory_iterator(folderToRecurse))
                        {
                        if (std::filesystem::exists(p) && p.is_directory())
                            {
                            excResults.excludedPaths.push_back(p.path().wstring());
                            }
                        }
                    }
                else
                    {
                    excResults.excludedFiles.push_back(relPath.wstring());
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
    std::vector<std::wstring> get_files_to_analyze(const std::filesystem::path& inputFolder,
                                                   const std::vector<std::wstring>& excludedPaths,
                                                   const std::vector<std::wstring>& excludedFiles)
        {
        std::vector<std::wstring> filesToAnalyze;

        if (std::filesystem::is_regular_file(inputFolder) && std::filesystem::exists(inputFolder))
            {
            filesToAnalyze.push_back(inputFolder.wstring());
            }
        else if (std::filesystem::is_directory(inputFolder) && std::filesystem::exists(inputFolder))
            {
            for (const auto& p : std::filesystem::recursive_directory_iterator(inputFolder))
                {
                const auto ext = p.path().extension();
                bool inExcludedPath{ false };
                for (const auto& ePath : excludedPaths)
                    {
                    std::filesystem::path excPath(ePath, std::filesystem::path::native_format);
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
                        std::filesystem::path excFile(eFile, std::filesystem::path::native_format);
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
                     ext.compare(std::filesystem::path(L".cpp")) == 0 ||
                     ext.compare(std::filesystem::path(L".h")) == 0 ||
                     ext.compare(std::filesystem::path(L".hpp")) == 0 ||
                     ext.compare(std::filesystem::path(L".po")) == 0) &&
                    // ignore CMake build files
                    p.path().filename().compare(L"CMakeCXXCompilerId.cpp") != 0 &&
                    p.path().filename().compare(L"CMakeCCompilerId.c") != 0)
                    {
                    filesToAnalyze.push_back(p.path().wstring());
                    }
                }
            }

        return filesToAnalyze;
        }
    } // namespace i18n_check
