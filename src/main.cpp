#include "cpp_i18n_review.h"
#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char *argv[])
    {
    i18n_check::cpp_i18n_review cpp;

    for (const auto& p : fs::recursive_directory_iterator(argv[1]))
        {
        std::wifstream ifs(p.path());
        std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)), std::istreambuf_iterator<wchar_t>());
        cpp(str.c_str(), str.length(), p.path());
        }

    cpp.review_localizable_strings();
    cpp.run_diagnostics();

    std::wstring report;
    for (const auto& val : cpp.get_localizable_strings_in_internal_call())
        {
        report.append(val.m_string).append(L"\n");
        }

    std::wcout << report;

    return 0;
    }
