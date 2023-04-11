///////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2023 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "cpp_i18n_review.h"
#include "cxxopts/include/cxxopts.hpp"
#include "utfcpp/source/utf8.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <numeric>

namespace fs = std::filesystem;
using namespace i18n_check;
using namespace string_util;
using namespace i18n_string_util;

//-------------------------------------------------
bool valid_utf8_file(const std::string& file_name)
    {
    std::ifstream ifs(file_name);
    if (!ifs)
        { return false; }

    std::istreambuf_iterator<char> it(ifs.rdbuf());
    std::istreambuf_iterator<char> eos;

    return utf8::is_valid(it, eos);
    }

//-------------------------------------------------
std::pair<bool, std::wstring> read_utf8_file(const std::string& file_name)
    {
    if (!valid_utf8_file(file_name))
        { return std::make_pair(false, std::wstring{}); }

    std::ifstream fs8(file_name);
    if (!fs8.is_open())
        {
        std::cout << "Could not open " << file_name << "\n";
        return std::make_pair(false, std::wstring{});
        }

    unsigned line_count = 1;
    std::string line;
    std::wstring buffer;
    buffer.reserve(static_cast<size_t>(std::filesystem::file_size(file_name)));

    while (std::getline(fs8, line))
        {
        if (auto end_it = utf8::find_invalid(line.begin(), line.end());
            end_it != line.cend())
            {
            std::cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
            std::cout << "This part is fine: " << std::string(line.begin(), end_it) << "\n";
            }

        // Convert it to utf-16
        std::u16string utf16line = utf8::utf8to16(line);
        for (const auto& ch : utf16line)
            { buffer += static_cast<wchar_t>(ch); }
        buffer += L"\n";

        ++line_count;
        }

    return std::make_pair(true, buffer);
    }

//-------------------------------------------------
int main(int argc, char* argv[])
    {
    cxxopts::Options options("i18n-check 1.0",
        "Internationalization/localization analysis system, (c) 2021-2023 Blake Madden");
    options.add_options()
    ("input", "The folder to analyze", cxxopts::value<std::string>())
    ("enable", "Which checks to perform (any combination of: "
        "all, suspectL10NString, suspectL10NUsage, urlInL10NString, notL10NAvailable, "
        "deprecatedMacros, nonUTF8File, unencodedExtASCII, printfSingleNumber)",
        cxxopts::value<std::vector<std::string>>())
    ("disable", "Which checks to not perform (same as the options for enable)",
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
    const auto readBoolOption =
        [&result]
        (const std::string& option, const bool defaultValue)
        {
        if (result.count(option) > 0)
            { return result[option].as<bool>(); }
        else
            { return defaultValue; }
        };

    const auto readIntOption =
        [&result]
        (const std::string& option, const int defaultValue)
        {
        if (result.count(option) > 0)
            { return result[option].as<int>(); }
        else
            { return defaultValue; }
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
        const auto ext = p.path().extension();
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
            (ext.compare(fs::path(L".c")) == 0 ||
             ext.compare(fs::path(L".cpp")) == 0 ||
             ext.compare(fs::path(L".h")) == 0 ||
             ext.compare(fs::path(L".hpp")) == 0))
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
            else if (r == "urlInL10NString")
                { rs |= check_l10n_contains_url; }
            else if (r == "notL10NAvailable")
                { rs |= check_not_available_for_l10n; }
            else if (r == "deprecatedMacros")
                { rs |= check_deprecated_macros; }
            else if (r == "nonUTF8File")
                { rs |= check_utf8_encoded; }
            else if (r == "unencodedExtASCII")
                { rs |= check_unencoded_ext_ascii; }
            else if (r == "printfSingleNumber")
                { rs |= check_printf_single_number; }
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
    // ...and if any checks are being excluded
    if (result.count("disable"))
        {
        const auto& styles =
            result["disable"].as<std::vector<std::string>>();
        int rs{ static_cast<int>(cpp.get_style()) };
        for (const auto& r : styles)
            {
            if (r == "all")
                {
                std::wcout << L"At least one check should be performed.";
                return 1;
                }
            else if (r == "suspectL10NString")
                { rs = rs & ~check_l10n_strings; }
            else if (r == "suspectL10NUsage")
                { rs = rs & ~check_suspect_l10n_string_usage; }
            else if (r == "urlInL10NString")
                { rs = rs & ~check_l10n_contains_url; }
            else if (r == "notL10NAvailable")
                { rs = rs & ~check_not_available_for_l10n; }
            else if (r == "deprecatedMacros")
                { rs = rs & ~check_deprecated_macros; }
            else if (r == "nonUTF8File")
                { rs = rs & ~check_utf8_encoded; }
            else if (r == "unencodedExtASCII")
                { rs = rs & ~check_unencoded_ext_ascii; }
            else if (r == "printfSingleNumber")
                { rs = rs & ~check_printf_single_number; }
            else
                {
                std::wcout << L"Unknown option passed to --disable: " <<
                    lazy_string_to_wstring(r) << L"\n\n" <<
                    lazy_string_to_wstring(options.help()) << L"\n";
                return 1;
                }
            }
        cpp.set_style(static_cast<i18n_check::review_style>(rs));
        }

    std::vector<std::wstring> filesThatShouldBeConvertedToUTF8;
    size_t currentFileIndex{ 0 };
    for (const auto& file : filesToAnalyze)
        {
        if (!readBoolOption("quiet", false))
            {
            std::wcout << L"Examining " << ++currentFileIndex <<
                L" of " << filesToAnalyze.size() << L" files (" <<
                fs::path(file).filename() << L")\n";
            }

        try
            {
            if (const auto [readOk, fileText] = read_utf8_file(file);
                readOk)
                {
                cpp(fileText.c_str(), fileText.length(), fs::path(file).wstring());
                }
            else
                {
                if (cpp.get_style() & check_utf8_encoded)
                    { filesThatShouldBeConvertedToUTF8.push_back(lazy_string_to_wstring(file)); }
                std::wifstream ifs(file);
                std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)),
                                std::istreambuf_iterator<wchar_t>());
                cpp(str.c_str(), str.length(), fs::path(file).wstring());
                }
            }
        catch (const std::exception& expt)
            { std::wcout << lazy_string_to_wstring(expt.what()) << L"\n"; }
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

    for (const auto& val : cpp.get_localizable_strings_with_urls())
        {
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t" <<
            L"\"" << val.m_string << L"\"\t";
        if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
            {
            report << L"String available for translation that contains an "
                "URL or email address in function call: " <<
                val.m_usage.m_value << L"\t";
            }
        else if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable)
            {
            report << L"String available for translation that contains an "
                "URL or email address assigned to variable: " <<
                val.m_usage.m_value << L"\t";
            }
        else
            {
            report << L"String available for translation that contains an "
                "URL or email address within " <<
                val.m_usage.m_value << L"\t";
            }
        report << L"[urlInL10NString]\n";
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
        const auto foundMessage = cpp.get_deprecated_messages().find(val.m_string);
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column <<
            L"\t" << val.m_string << L"\t" <<
            (foundMessage == cpp.get_deprecated_messages().cend() ?
                L"Deprecated function should be replaced." : foundMessage->second) <<
                L"\t[deprecatedMacro]\n";
        }

    for (const auto& val : cpp.get_printf_single_numbers())
        {
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column <<
            L"\t" << val.m_string << L"\t" <<
            L"Prefer using std::to_[w]string() instead of printf() formatting a single number."<<
            L"\t[printfSingleNumber]\n";
        }

    for (const auto& file : filesThatShouldBeConvertedToUTF8)
        {
        report << file <<
            L"\t\t\t\tFile contains extended ASCII characters, but is not encoded as UTF-8.\t[nonUTF8File]\n";
        }

    for (const auto& val : cpp.get_unencoded_ext_ascii_strings())
        {
        std::wstringstream encodingRecommendationsStream;
        for (const auto& ch : val.m_string)
            {
            if (ch > 127)
                {
                encodingRecommendationsStream << ch << L" -> \\U" <<
                    std::setfill(L'0') << std::setw(8) << std::uppercase <<
                    std::hex  <<
                    static_cast<int>(ch) << L", ";
                }
            }
        auto encodingRecommendations = encodingRecommendationsStream.str();
        if (encodingRecommendations.length() > 2)
            {
            encodingRecommendations.erase(encodingRecommendations.end()-2,
                                          encodingRecommendations.end());
            }
        report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t" <<
            L"\"" << val.m_string << L"\"\t" <<
            L"String contains extended ASCII characters that should be encoded. Recommended changes: " <<
            encodingRecommendations <<
            L"\t[unencodedExtASCII]\n";
        }

    if (readBoolOption("verbose", false))
        {
        for (const auto& parseErr : cpp.get_error_log())
            {
            report << parseErr.m_file_name << L"\t";
            if (parseErr.m_line != std::wstring::npos)
                { report << parseErr.m_line; }
            report << L"\t";
            if (parseErr.m_column != std::wstring::npos)
                { report << parseErr.m_column; }
            report << L"\t" <<
                parseErr.m_resourceString << L"\t" << parseErr.m_message << L"\t[debugParserInfo]\n";
            }
        }

    // write the output
    if (result.count("output"))
        {
        fs::path outPath{ result["output"].as<std::string>() };
        std::ofstream ofs(outPath);

        // write the results report in UTF-8
        std::string utf8Str;
        auto resText = report.str();
        if constexpr (sizeof(wchar_t) == sizeof(uint16_t))
            { utf8::utf16to8(resText.begin(), resText.end(), back_inserter(utf8Str)); }
        else if constexpr (sizeof(wchar_t) == sizeof(uint32_t))
            { utf8::utf32to8(resText.begin(), resText.end(), back_inserter(utf8Str)); }

        ofs << utf8Str;
        }
    else
        { std::wcout << report.str(); }

    if (!readBoolOption("quiet", false))
        {
        const auto endTime{ std::chrono::high_resolution_clock::now() };

        if (std::chrono::duration_cast<std::chrono::seconds>
            (endTime - startTime).count() < 1)
            {
            std::wcout << L"\nFinished in " <<
                std::chrono::duration_cast<std::chrono::milliseconds>
                    (endTime - startTime).count() <<
                L" milliseconds.";
            }
        else
            {
            std::wcout << L"\nFinished in " <<
                std::chrono::duration_cast<std::chrono::seconds>
                    (endTime - startTime).count() <<
                L" seconds.\n";
            }
        }

    return 0;
    }
