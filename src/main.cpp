#include "cpp_i18n_review.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace i18n_check;

int main(int argc, char *argv[])
    {
    i18n_check::cpp_i18n_review cpp;

    std::wstringstream report;

    for (size_t i = 1; i < argc; ++i)
        {
        for (const auto& p : fs::recursive_directory_iterator(argv[i]))
            {
            std::wifstream ifs(p.path());
            std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)), std::istreambuf_iterator<wchar_t>());
            string_util::case_insensitive_wstring ext = p.path().extension().c_str();
            if (ext == L".c" || ext == L".cpp" || ext == L".h" || ext == L".hpp")
                {
                cpp(str.c_str(), str.length(), p.path());
                std::wcout << L"#";
                }
            }
        }

    cpp.review_localizable_strings();
    cpp.run_diagnostics();

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
