///////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2023 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "cpp_i18n_review.h"
#include "cxxopts/include/cxxopts.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <numeric>

namespace fs = std::filesystem;
using namespace i18n_check;
using namespace string_util;
using namespace i18n_string_util;

int main(int argc, char* argv[])
    {
    cxxopts::Options options("i18n-check 1.0",
        "Internationalization/localization analysis system, (c) 2021-2023 Blake Madden");
    options.add_options()
    ("input", "The folder to analyze", cxxopts::value<std::string>())
    ("enable", "Which checks to perform (any combination of: "
        "all, suspectL10NString, suspectL10NUsage, notL10NAvailable, deprecatedMacros)",
        cxxopts::value<std::vector<std::string>>())
    ("log-l10n-allowed", "Whether it is acceptable to pass translatable "
        "strings to logging functions. (Default is true.)")
    ("punct-l10n-allowed", "Whether it is acceptable for punctuation only "
        "strings to be translatable. (Default is false.)",
        cxxopts::value<bool>()->default_value("false"))
    ("exceptions-l10n-required", "Whether to verify that exception messages are available for translation.")
    ("min-l10n-wordcount",
        "The minimum number of words that a string must have to be reviewed for whether it should be translatable.",
        cxxopts::value<int>())
    ("i,ignore", "Folders and files to ignore (can be used multiple times)",
        cxxopts::value<std::vector<std::string>>())
    ("o,output", "The output report path", cxxopts::value<std::string>())
    ("q,quiet", "Only print errors and the final output", cxxopts::value<bool>()->default_value("false"))
    ("v,verbose", "Display debug information", cxxopts::value<bool>()->default_value("false"))
    ("h,help", "Print usage");

    const auto startTime{ std::chrono::high_resolution_clock::now() };

    cxxopts::ParseResult result;
    try
        {
        options.parse_positional({"input"});

        result = options.parse(argc, argv);

        if (result.count("help"))
            {
            std::wcout << lazy_string_to_wstring(options.help()) << L"\n";
            return 0;
            }
        }
    catch (const cxxopts::exceptions::exception& exp)
        {
        std::wcout << lazy_string_to_wstring(exp.what());
        return 0;
        }

    // helper to get a boolean option (option not being present returns default)
    const auto readBoolOption = [&result](const auto& option, const bool default)
        {
        if (result.count(option) > 0)
            { return result[option].as<bool>(); }
        else
            { return default; }
        };

    const auto readIntOption = [&result](const auto& option, const int default)
        {
        if (result.count(option) > 0)
            { return result[option].as<int>(); }
        else
            { return default; }
        };

    fs::path inputFolder;
    if (result.count("input"))
        {
        inputFolder = fs::path{ result["input"].as<std::string>(),
                                fs::path::native_format };
        if (!fs::exists(inputFolder))
            {
            std::wcout << L"Input path does not exist: " << inputFolder;
            return 0;
            }
        }
    else
        {
        std::wcout << L"You must pass in at least one folder to analyze.";
        return 0;
        }

    // paths being ignored
    std::vector<std::string> excludedPaths;
    std::vector<std::string> excludedFiles;
    if (result.count("ignore"))
        {
        const auto& providedExcFolders =
            result["ignore"].as<std::vector<std::string>>();
        for (const auto& excFolder : providedExcFolders)
            {
            std::error_code ec;
            if (fs::exists(excFolder))
                {
                if (fs::is_directory(excFolder))
                    {
                    excludedPaths.push_back(excFolder);
                    const auto folderToRecurse{ excludedPaths.back() };
                    // add subdirectories
                    for (const auto& p :
                        fs::recursive_directory_iterator(folderToRecurse))
                        {
                        if (fs::exists(p) && p.is_directory())
                            { excludedPaths.push_back(p.path().string()); }
                        }
                    }
                else
                    { excludedFiles.push_back(excFolder); }
                }
            // if not a full path, just a subdirectory path
            else if (const auto relPath =
                fs::path{ inputFolder } / excFolder;
                fs::exists(relPath))
                {
                if (fs::is_directory(relPath))
                    {
                    excludedPaths.push_back(relPath.string());
                    const auto folderToRecurse{ excludedPaths.back() };
                    // add subdirectories
                    for (const auto& p :
                        fs::recursive_directory_iterator(folderToRecurse))
                        {
                        if (fs::exists(p) && p.is_directory())
                            { excludedPaths.push_back(p.path().string()); }
                        }
                    }
                else
                    { excludedFiles.push_back(relPath.string()); }
                }
            else
                { continue; }
            }
        }

    // input folder
    if (!readBoolOption("quiet", false))
        {
        std::wcout << L"\n###################################################\n# " <<
            lazy_string_to_wstring(options.program()) <<
            L":\n# Internationalization/localization analysis system\n# (c) 2021-2023 Blake Madden\n" <<
            L"###################################################\n\n";
        std::wcout << L"Searching for files to analyze in " << inputFolder << L"...\n\n";
        }
    std::vector<std::string> filesToAnalyze; 
    
    for (const auto& p :
        fs::recursive_directory_iterator(inputFolder))
        {
        case_insensitive_wstring ext{ p.path().extension().c_str() };
        bool inExcludedPath{ false };
        for (const auto& ePath : excludedPaths)
            {
            fs::path excPath(ePath, fs::path::native_format);
            if (p.exists() && fs::exists(excPath) &&
                fs::equivalent(p.path().parent_path(), excPath) )
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
                fs::path excFile(eFile, fs::path::native_format);
                if (p.exists() && fs::exists(excFile) &&
                    fs::equivalent(p, excFile) )
                    {
                    inExcludedPath = true;
                    break;
                    }
                }
            }
        if (p.exists() && p.is_regular_file() &&
            !inExcludedPath &&
            (ext == L".c" || ext == L".cpp" || ext == L".h" || ext == L".hpp"))
            {
            filesToAnalyze.push_back(p.path().string());
            }
        }
        
    i18n_check::cpp_i18n_review cpp;
    cpp.log_messages_can_be_translatable(readBoolOption("log-l10n-allowed", true));
    cpp.allow_translating_punctuation_only_strings(readBoolOption("punct-l10n-allowed", false));
    cpp.exceptions_should_be_translatable(readBoolOption("exceptions-l10n-required", true));
    cpp.set_min_words_for_classifying_unavailable_string(readIntOption("min-l10n-wordcount", 2));
    cpp.reserve(filesToAnalyze.size());
    // see which checks are being performed
    if (result.count("enable"))
        {
        const auto& styles =
            result["enable"].as<std::vector<std::string>>();
        int rs{ no_l10n_checks };
        for (const auto& r : styles)
            {
            if (r == "all")
                {
                rs = i18n_check::review_style::all_l10n_checks;
                break;
                }
            else if (r == "suspectL10NString")
                { rs |= check_l10n_strings; }
            else if (r == "suspectL10NUsage")
                { rs |= check_suspect_l10n_string_usage; }
            else if (r == "notL10NAvailable")
                { rs |= check_not_available_for_l10n; }
            else if (r == "deprecatedMacros")
                { rs |= check_deprecated_macros; }
            else
                {
                std::wcout << L"Unknown option passed to --enable: " <<
                    lazy_string_to_wstring(r) << L"\n\n" <<
                    lazy_string_to_wstring(options.help()) << L"\n";
                return 1;
                }
            }
        cpp.set_style(static_cast<i18n_check::review_style>(rs));
        }

    size_t currentFileIndex{ 0 };
    for (const auto& file : filesToAnalyze)
        {
        std::wifstream ifs(file);
        std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)),
                          std::istreambuf_iterator<wchar_t>());

        if (!readBoolOption("quiet", false))
            {
            std::wcout << L"Examining " << ++currentFileIndex <<
                L" of " << filesToAnalyze.size() << L" files (" <<
                fs::path(file).filename() << L")\n";
            }
        cpp(str.c_str(), str.length(), fs::path(file).wstring());
        }

    if (!readBoolOption("quiet", false))
        { std::wcout << L"Reviewing strings...\n"; }
    cpp.review_strings();

    std::wstringstream report;
    report << "File\tLine\tColumn\tValue\tExplanation\tWarningID\n";
    for (const auto& val : cpp.get_unsafe_localizable_strings())
        {
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t" <<
            L"\"" << val.m_string << L"\"\t";
        if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
            {
            report << L"String available for translation that probably "
                "should not be in function call: " <<
                val.m_usage.m_value << L"\t";
            }
        else if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable)
            {
            report << L"String available for translation that probably "
                "should not be assigned to variable: " <<
                val.m_usage.m_value << L"\t";
            }
        else
            {
            report << L"String available for translation that probably "
                "should not be within " <<
                val.m_usage.m_value << L"\t";
            }
        report << L"[suspectL10NString]\n";
        }
    
    for (const auto& val : cpp.get_localizable_strings_in_internal_call())
        {
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t" <<
            L"\"" << val.m_string << L"\"\t";
        if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
            {
            report << L"Localizable string being used within non-user facing function call: " <<
                val.m_usage.m_value << L"\t";
            }
        else if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable)
            {
            report << L"Localizable string being assigned to non-user facing variable: " <<
                val.m_usage.m_value << L"\t";
            }
        else
            {
            report << L"Localizable string being assigned to " <<
                val.m_usage.m_value << L"\t";
            }
        report << L"[suspectL10NUsage]\n";
        }

    for (const auto& val : cpp.get_not_available_for_localization_strings())
        {
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t" <<
            L"\"" << val.m_string << L"\"\t";
        if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
            {
            report << L"String not available for translation in function call: " <<
                val.m_usage.m_value << L"\t";
            }
        else if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable)
            {
            report << L"String not available for translation assigned to variable: " <<
                val.m_usage.m_value << L"\t";
            }
        else
            {
            report << L"String not available for translation assigned to " <<
                val.m_usage.m_value << L"\t";
            }
        report << L"[notL10NAvailable]\n";
        }

    for (const auto& val : cpp.get_deprecated_macros())
        {
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t" <<
            L"\"" << val.m_string << L"\"\t" <<
            L"Deprecated text macro that can be removed." <<
            val.m_usage.m_value << L"\t[deprecatedMacro]\n";
        }

    if (readBoolOption("verbose", false))
        {
        for (const auto& parseErr : cpp.get_error_log())
            {
            report << parseErr.m_file_name << L"\t\t\t" <<
                parseErr.m_resourceString << L"\t" << parseErr.m_message << L"\t[debugParserInfo]\n";
            }
        }

    // write the output
    if (result.count("output"))
        {
        fs::path outPath{ result["output"].as<std::string>() };
        std::wofstream ofs(outPath);
        ofs << report.str();
        }
    else
        { std::wcout << report.str(); }

    const auto endTime{ std::chrono::high_resolution_clock::now() };

    if (std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() < 1)
        {
        std::wcout << L"\nFinished in " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() <<
            L" milliseconds.";
        }
    else
        {
        std::wcout << L"\nFinished in " <<
            std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() <<
            L" seconds.";
        }

    return 0;
    }
