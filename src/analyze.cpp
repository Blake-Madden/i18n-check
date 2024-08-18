//////////////////////////////////////////////////////////////////////////////
// Name:        analyze.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "analyze.h"
#include <iostream>

namespace i18n_check
    {
    //------------------------------------------------------
    bool valid_utf8_file(const std::wstring& file_name, bool& startsWithBom)
        {
        startsWithBom = false;
        std::wifstream ifs(file_name);
        if (!ifs)
            {
            return false;
            }

        std::istreambuf_iterator<wchar_t> it(ifs.rdbuf());
        std::istreambuf_iterator<wchar_t> eos;

        if (utf8::starts_with_bom(it, eos))
            {
            startsWithBom = true;
            std::advance(it, std::size(utf8::bom));
            }

        return utf8::is_valid(it, eos);
        }

    //------------------------------------------------------
    std::pair<bool, std::wstring> read_utf16_file(const std::wstring& file_name)
        {
        std::ifstream fs8(file_name);
        if (!fs8.is_open())
            {
            std::wcout << L"Could not open " << file_name << L"\n";
            return std::make_pair(false, std::wstring{});
            }

        std::string line;
        std::string buffer;
        buffer.reserve(static_cast<size_t>(std::filesystem::file_size(file_name)));

        fs8.seekg(0, std::ios_base::end);
        const auto fileLength = fs8.tellg();
        fs8.seekg(0);
        std::vector<char> fileData(static_cast<size_t>(fileLength));
        fs8.read(&fileData[0], fileLength);

        if (lily_of_the_valley::unicode_extract_text::is_unicode(&fileData[0]))
            {
            lily_of_the_valley::unicode_extract_text uExtract;
            uExtract(&fileData[0], fileData.size(),
                     lily_of_the_valley::unicode_extract_text::is_little_endian(&fileData[0]));
            return std::make_pair(true, std::wstring{ uExtract.get_filtered_text() });
            }
        return std::make_pair(false, std::wstring{});
        }

    //------------------------------------------------------
    std::pair<bool, std::wstring> read_utf8_file(const std::wstring& file_name, bool& startsWithBom)
        {
        if (!valid_utf8_file(file_name, startsWithBom))
            {
            return std::make_pair(false, std::wstring{});
            }

        std::ifstream fs8(file_name);
        if (!fs8.is_open())
            {
            std::wcout << L"Could not open " << file_name << L"\n";
            return std::make_pair(false, std::wstring{});
            }

        unsigned line_count = 1;
        std::string line;
        std::wstring buffer;
        buffer.reserve(static_cast<size_t>(std::filesystem::file_size(file_name)));

        while (std::getline(fs8, line))
            {
            if (auto end_it = utf8::find_invalid(line.begin(), line.end()); end_it != line.cend())
                {
                std::cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
                std::cout << "This part is fine: " << std::string(line.begin(), end_it) << "\n";
                }

            // Convert it to utf-16
            std::u16string utf16line = utf8::utf8to16(line);
            for (const auto& ch : utf16line)
                {
                buffer += static_cast<wchar_t>(ch);
                }
            buffer += L"\n";

            ++line_count;
            }

        return std::make_pair(true, buffer);
        }

    //------------------------------------------------------
    void analyze(const std::vector<std::wstring>& filesToAnalyze, i18n_check::cpp_i18n_review& cpp,
                 i18n_check::rc_file_review& rc, i18n_check::po_file_review& po,
                 std::vector<std::wstring>& filesThatShouldBeConvertedToUTF8,
                 std::vector<std::wstring>& filesThatContainUTF8Signature,
                 analyze_callback callback)
        {
        filesThatShouldBeConvertedToUTF8.clear();
        filesThatContainUTF8Signature.clear();
        cpp.clear_results();
        cpp.reserve(filesToAnalyze.size() / 3);
        rc.clear_results();
        rc.reserve(filesToAnalyze.size() / 3);
        po.clear_results();
        po.reserve(filesToAnalyze.size() / 3);

        size_t currentFileIndex{ 0 };

        // load file content into analyzers
        for (const auto& file : filesToAnalyze)
            {
            callback(++currentFileIndex, filesToAnalyze.size(), file);

            const file_review_type fileType = [&file]()
            {
                const auto ext = std::filesystem::path{ file }.extension();
                if (ext.compare(std::filesystem::path(L".rc")) == 0)
                    {
                    return file_review_type::rc;
                    }
                else if (ext.compare(std::filesystem::path(L".po")) == 0)
                    {
                    return file_review_type::po;
                    }
                else
                    {
                    return file_review_type::cpp;
                    }
            }();

            try
                {
                bool startsWithBom{ false };
                if (const auto [readUtf8Ok, fileUtf8Text] = read_utf8_file(file, startsWithBom);
                    readUtf8Ok)
                    {
                    if (startsWithBom && cpp.get_style() & check_utf8_with_signature)
                        {
                        filesThatContainUTF8Signature.push_back(file);
                        }
                    if (fileType == file_review_type::rc)
                        {
                        rc(fileUtf8Text, std::filesystem::path(file).wstring());
                        }
                    else if (fileType == file_review_type::po)
                        {
                        po(fileUtf8Text, std::filesystem::path(file).wstring());
                        }
                    else
                        {
                        cpp(fileUtf8Text, std::filesystem::path(file).wstring());
                        }
                    }
                else if (const auto [readUtf16Ok, fileUtf16Text] = read_utf16_file(file);
                         readUtf16Ok)
                    {
                    // UTF-16 may not be supported consistently on all platforms and compilers
                    if (cpp.get_style() & check_utf8_encoded)
                        {
                        filesThatShouldBeConvertedToUTF8.push_back(file);
                        }
                    if (fileType == file_review_type::rc)
                        {
                        rc(fileUtf16Text, std::filesystem::path(file).wstring());
                        }
                    else if (fileType == file_review_type::po)
                        {
                        po(fileUtf16Text, std::filesystem::path(file).wstring());
                        }
                    else
                        {
                        cpp(fileUtf16Text, std::filesystem::path(file).wstring());
                        }
                    }
                else
                    {
                    if (cpp.get_style() & check_utf8_encoded)
                        {
                        filesThatShouldBeConvertedToUTF8.push_back(file);
                        }
                    std::wifstream ifs(file);
                    const std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)),
                                           std::istreambuf_iterator<wchar_t>());
                    if (fileType == file_review_type::rc)
                        {
                        rc(str, std::filesystem::path(file).wstring());
                        }
                    else if (fileType == file_review_type::po)
                        {
                        po(str, std::filesystem::path(file).wstring());
                        }
                    else
                        {
                        cpp(str, std::filesystem::path(file).wstring());
                        }
                    }
                }
            catch (const std::exception& expt)
                {
                std::wcout << i18n_string_util::lazy_string_to_wstring(expt.what()) << L"\n";
                }
            }

        // analyze the content
        cpp.review_strings();
        po.review_strings();
        }

    //------------------------------------------------------
    std::wstringstream format_summary(const i18n_check::cpp_i18n_review& cpp,
                                      const i18n_check::rc_file_review& rc,
                                      const i18n_check::po_file_review& po)
        {
        std::wstringstream report;
        report << L"Checks Performed" << L"\n###################################################\n"
               << ((cpp.get_style() & check_l10n_strings) ? L"suspectL10NString\n" : L"")
               << ((cpp.get_style() & check_suspect_l10n_string_usage) ? L"suspectL10NUsage\n" :
                                                                         L"")
               << ((cpp.get_style() & check_mismatching_printf_commands) ? L"printfMismatch\n" :
                                                                           L"")
               << ((cpp.get_style() & check_l10n_contains_url) ? L"urlInL10NString\n" : L"")
               << ((cpp.get_style() & check_not_available_for_l10n) ? L"notL10NAvailable\n" : L"")
               << ((cpp.get_style() & check_deprecated_macros) ? L"deprecatedMacro\n" : L"")
               << ((cpp.get_style() & check_utf8_encoded) ? L"nonUTF8File\n" : L"")
               << ((cpp.get_style() & check_utf8_with_signature) ? L"UTF8FileWithBOM\n" : L"")
               << ((cpp.get_style() & check_unencoded_ext_ascii) ? L"unencodedExtASCII\n" : L"")
               << ((cpp.get_style() & check_printf_single_number) ? L"printfSingleNumber\n" : L"")
               << ((cpp.get_style() & check_number_assigned_to_id) ? L"numberAssignedToId\n" : L"")
               << ((cpp.get_style() & check_duplicate_value_assigned_to_ids) ?
                       L"dupValAssignedToIds\n" :
                       L"")
               << ((cpp.get_style() & check_malformed_strings) ? L"malformedString\n" : L"")
               << ((cpp.get_style() & check_fonts) ? L"fontIssue\n" : L"")
               << ((cpp.get_style() & check_trailing_spaces) ? L"trailingSpaces\n" : L"")
               << ((cpp.get_style() & check_tabs) ? L"tabs\n" : L"")
               << ((cpp.get_style() & check_line_width) ? L"wideLine\n" : L"")
               << ((cpp.get_style() & check_space_after_comment) ? L"commentMissingSpace\n" : L"")
               << L"\n"
               << L"Statistics" << L"\n###################################################\n"
               << L"Strings available for translation within C/C++ source files: "
               << cpp.get_localizable_strings().size() << L"\n"
               << L"String table entries within Windows resource files: "
               << rc.get_localizable_strings().size() << L"\n"
               << L"Translation entries within PO message catalog files: "
               << po.get_catalog_entries().size() << L"\n";

        return report;
        }

    //------------------------------------------------------
    std::wstringstream format_results(i18n_check::cpp_i18n_review& cpp,
                                      i18n_check::rc_file_review& rc,
                                      i18n_check::po_file_review& po,
                                      std::vector<std::wstring>& filesThatShouldBeConvertedToUTF8,
                                      std::vector<std::wstring>& filesThatContainUTF8Signature,
                                      const bool verbose /*= false*/)
        {
        const auto replaceSpecialSpaces = [](const std::wstring& str)
        {
            auto newStr{ str };
            string_util::replace_all(newStr, L'\t', L' ');
            string_util::replace_all(newStr, L'\n', L' ');
            string_util::replace_all(newStr, L'\r', L' ');
            return newStr;
        };

        /* Note: yes, I am aware of the irony of i18n bad practices here :)
           Normally, you shouldn't piece strings together, should make them available for
           translation, etc. However, I am just keeping this tool simple.*/
        std::wstringstream report;
        report << "File\tLine\tColumn\tValue\tExplanation\tWarningID\n";

        // Windows resource file warnings
        for (const auto& val : rc.get_unsafe_localizable_strings())
            {
            report << val.m_file_name << L"\t\t\t" << L"\"" << replaceSpecialSpaces(val.m_string)
                   << L"\"\t" << L"String available for translation that probably should not be"
                   << L"\t[suspectL10NString]\n";
            }

        for (const auto& val : rc.get_bad_dialog_font_sizes())
            {
            report << val.m_file_name << L"\t\t\t" << L"\"" << replaceSpecialSpaces(val.m_string)
                   << L"\"\t" << L"Font issue in resource file dialog definition."
                   << L"\t[fontIssue]\n";
            }

        for (const auto& val : rc.get_non_system_dialog_fonts())
            {
            report << val.m_file_name << L"\t\t\t" << L"\"" << replaceSpecialSpaces(val.m_string)
                   << L"\"\t" << L"Font issue in resource file dialog definition."
                   << L"\t[fontIssue]\n";
            }

        // C/C++ warnings
        for (const auto& val : cpp.get_unsafe_localizable_strings())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                   << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t";
            if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
                {
                report << L"String available for translation that probably "
                          "should not be in function call: "
                       << val.m_usage.m_value << L"\t";
                }
            else if (val.m_usage.m_type ==
                     i18n_review::string_info::usage_info::usage_type::variable)
                {
                report << L"String available for translation that probably "
                          "should not be assigned to variable: "
                       << val.m_usage.m_value << L"\t";
                }
            else
                {
                report << L"String available for translation that probably "
                          "should not be within "
                       << val.m_usage.m_value << L"\t";
                }
            report << L"[suspectL10NString]\n";
            }

        for (const auto& val : cpp.get_localizable_strings_with_urls())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                   << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t";
            if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
                {
                report << L"String available for translation that contains an "
                          "URL or email address in function call: "
                       << val.m_usage.m_value << L"\t";
                }
            else if (val.m_usage.m_type ==
                     i18n_review::string_info::usage_info::usage_type::variable)
                {
                report << L"String available for translation that contains an "
                          "URL or email address assigned to variable: "
                       << val.m_usage.m_value << L"\t";
                }
            else
                {
                report << L"String available for translation that contains an "
                          "URL or email address within "
                       << val.m_usage.m_value << L"\t";
                }
            report << L"[urlInL10NString]\n";
            }

        for (const auto& val : cpp.get_localizable_strings_in_internal_call())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                   << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t";
            if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
                {
                report << L"Localizable string being used within non-user facing function call: "
                       << val.m_usage.m_value << L"\t";
                }
            else if (val.m_usage.m_type ==
                     i18n_review::string_info::usage_info::usage_type::variable)
                {
                report << L"Localizable string being assigned to non-user facing variable: "
                       << val.m_usage.m_value << L"\t";
                }
            else
                {
                report << L"Localizable string being assigned to " << val.m_usage.m_value << L"\t";
                }
            report << L"[suspectL10NUsage]\n";
            }

        for (const auto& val : cpp.get_not_available_for_localization_strings())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                   << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t";
            if (val.m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function)
                {
                report << L"String not available for translation in function call: "
                       << val.m_usage.m_value << L"\t";
                }
            else if (val.m_usage.m_type ==
                     i18n_review::string_info::usage_info::usage_type::variable)
                {
                report << L"String not available for translation assigned to variable: "
                       << val.m_usage.m_value << L"\t";
                }
            else
                {
                report << L"String not available for translation assigned to "
                       << val.m_usage.m_value << L"\t";
                }
            report << L"[notL10NAvailable]\n";
            }

        for (const auto& val : cpp.get_deprecated_macros())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                   << replaceSpecialSpaces(val.m_string) << L"\t" << val.m_usage.m_value
                   << L"\t[deprecatedMacro]\n";
            }

        for (const auto& val : cpp.get_printf_single_numbers())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t"
                   << L"Prefer using std::to_[w]string() instead of printf() formatting a single "
                      L"number."
                   << L"\t[printfSingleNumber]\n";
            }

        for (const auto& val : cpp.get_duplicates_value_assigned_to_ids())
            {
            report << val.m_file_name << L"\t\t\t" << replaceSpecialSpaces(val.m_string) << L"\t"
                   << L"Verify that duplicate assignment was intended. "
                      "If correct, consider assigning the first ID variable by name "
                      "to the second one to make this intention clear."
                   << L"\t[dupValAssignedToIds]\n";
            }

        for (const auto& val : cpp.get_ids_assigned_number())
            {
            report << val.m_file_name << L"\t\t\t" << replaceSpecialSpaces(val.m_string) << L"\t"
                   << L"Prefer using ID constants provided by your framework when "
                      "assigning values to an ID variable."
                   << L"\t[numberAssignedToId]\n";
            }

        for (const auto& val : cpp.get_malformed_strings())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t"
                   << L"Malformed syntax in string." << L"\t[malformedString]\n";
            }

        for (const auto& file : filesThatShouldBeConvertedToUTF8)
            {
            report << file
                   << L"\t\t\t\tFile contains extended ASCII characters, "
                      "but is not encoded as UTF-8.\t[nonUTF8File]\n";
            }

        for (const auto& file : filesThatContainUTF8Signature)
            {
            report << file
                   << L"\t\t\t\tFile contains UTF-8 signature; "
                      "It is recommended to save without the file signature for "
                      "best compiler portability.\t[UTF8FileWithBOM]\n";
            }

        for (const auto& val : cpp.get_unencoded_ext_ascii_strings())
            {
            std::wstringstream encodingRecommendations;
            auto untabbedStr{ replaceSpecialSpaces(val.m_string) };
            for (const auto& ch : untabbedStr)
                {
                if (ch > 127)
                    {
                    encodingRecommendations << LR"(\U)" << std::setfill(L'0') << std::setw(8)
                                            << std::uppercase << std::hex << static_cast<int>(ch);
                    }
                else
                    {
                    encodingRecommendations << ch;
                    }
                }

            report
                << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t" << L"\""
                << replaceSpecialSpaces(val.m_string) << L"\"\t"
                << L"String contains extended ASCII characters that should be encoded. Recommended "
                   L"change: '"
                << encodingRecommendations.str() << L"'\t[unencodedExtASCII]\n";
            }

        for (const auto& val : cpp.get_trailing_spaces())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t"
                   << L"Trailing space(s) detected at end of line." << L"\t[trailingSpaces]\n";
            }

        for (const auto& val : cpp.get_tabs())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t"
                   << L"Tab detected in file; prefer using spaces." << L"\t[tabs]\n";
            }

        for (const auto& val : cpp.get_wide_lines())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t" << L"Line is "
                   << val.m_usage.m_value << L" characters long." << L"\t[wideLine]\n";
            }

        for (const auto& val : cpp.get_comments_missing_space())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t"
                   << L"Space should be inserted between comment tag and comment."
                   << L"\t[commentMissingSpace]\n";
            }

        for (const auto& catEntry : po.get_catalog_entries())
            {
            for (const auto& issue : catEntry.second.m_issues)
                {
                if (issue.first == translation_issue::printf_issue)
                    {
                    report
                        << catEntry.first << L"\t\t\t" << issue.second
                        << L"\tMismatching printf command between source and translation strings."
                           "\t[printfMismatch]\n";
                    }
                }
            }

        if (verbose)
            {
            for (const auto& parseErr : cpp.get_error_log())
                {
                report << parseErr.m_file_name << L"\t";
                if (parseErr.m_line != std::wstring::npos)
                    {
                    report << parseErr.m_line;
                    }
                report << L"\t";
                if (parseErr.m_column != std::wstring::npos)
                    {
                    report << parseErr.m_column;
                    }
                report << L"\t" << parseErr.m_resourceString << L"\t" << parseErr.m_message
                       << L"\t[debugParserInfo]\n";
                }
            }

        return report;
        }
    } // namespace i18n_check
