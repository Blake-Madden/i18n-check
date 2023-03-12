///////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "cpp_i18n_review.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace i18n_check;
using namespace string_util;

int wmain(int argc, wchar_t *argv[])
    {
    i18n_check::cpp_i18n_review cpp;

    if (argc < 2)
        {
        std::wcout << L"Not enough command line arguments.";
        return 0;
        }

    std::vector<std::wstring> excludedPaths;
    std::vector<std::wstring> pathsToAnalyze;

    for (size_t i = 1; i < argc; ++i)
        {
        if (std::wcslen(argv[i]) >= 3 && std::wcsncmp(argv[i], L"-i", 2) == 0)
            { excludedPaths.emplace_back(argv[i]+2); }
        else
            { pathsToAnalyze.emplace_back(argv[i]); }
        }    

    if (pathsToAnalyze.empty())
        {
        std::wcout << L"You must pass in at least one folder path to analyze.";
        return 0;
        }

    for (const auto& path : pathsToAnalyze)
        {
        for (const auto& p : fs::recursive_directory_iterator(path))
            {
            std::wifstream ifs(p.path());
            std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)), std::istreambuf_iterator<wchar_t>());
            case_insensitive_wstring ext{ p.path().extension().c_str() };
            case_insensitive_wstring currentPath{ p.path().parent_path().wstring().c_str() };
            bool inExcludedPath{ false };
            for (const auto& ePath : excludedPaths)
                {
                if (currentPath.compare(0, ePath.length(), ePath.c_str()) == 0)
                    {
                    inExcludedPath = true;
                    break;
                    }
                }
            if (!inExcludedPath &&
                (ext == L".c" || ext == L".cpp" || ext == L".h" || ext == L".hpp"))
                {
                cpp(str.c_str(), str.length(), p.path());
                std::wcout << L"#";
                }
            }
        }

    cpp.review_localizable_strings();
    cpp.run_diagnostics();

    std::wstringstream report;
    if (cpp.get_unsafe_localizable_strings().size())
        {
        report << L"Localizable strings that probably should not be\n" <<
                  L"===================================================================================\n\n";
        }
    for (const auto& val : cpp.get_unsafe_localizable_strings())
        {
        report << L"\"" << val.m_string << L"\"\n\t" <<
            val.m_file_name << L" (line " << val.m_line << L", column " << val.m_column << L")\n\t";
        if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
            { report << L"In function call: " << val.m_usage.m_value << L"\n\n"; }
        else if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable)
            { report << L"Assigned to variable: " << val.m_usage.m_value << L"\n\n"; }
        else
            { report << val.m_usage.m_value << L"\n\n"; }                  
        }
    
    if (cpp.get_localizable_strings_in_internal_call().size())
        {
        report << L"Strings being used with internal functions/variables that should not be localizable\n" <<
                  L"===================================================================================\n\n";
        }
    for (const auto& val : cpp.get_localizable_strings_in_internal_call())
        {
        report << L"\"" << val.m_string << L"\"\n\t" <<
            val.m_file_name << L" (line " << val.m_line << L", column " << val.m_column << L")\n\t";
        if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
            { report << L"In non-localizable function call: " << val.m_usage.m_value << L"\n\n"; }
        else if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable)
            { report << L"Assigned to non-localizable variable: " << val.m_usage.m_value << L"\n\n"; }
        else
            { report << val.m_usage.m_value << L"\n\n"; }                  
        }

    if (cpp.get_not_available_for_localization_strings().size())
        {
        report << L"Strings not available for translation\n" <<
                  L"===================================================================================\n\n";
        }
    for (const auto& val : cpp.get_not_available_for_localization_strings())
        {
        report << L"\"" << val.m_string << L"\"\n\t" <<
            val.m_file_name << L" (line " << val.m_line << L", column " << val.m_column << L")\n\t";
        if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
            { report << L"In function call: " << val.m_usage.m_value << L"\n\n"; }
        else if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable)
            { report << L"Assigned to variable: " << val.m_usage.m_value << L"\n\n"; }
        else
            { report << val.m_usage.m_value << L"\n\n"; }                  
        }

    std::wofstream ofs(L"output.txt");
    ofs << report.str().c_str();

    return 0;
    }
