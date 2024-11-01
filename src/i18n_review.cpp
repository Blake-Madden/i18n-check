///////////////////////////////////////////////////////////////////////////////
// Name:        i18n_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "i18n_review.h"

namespace i18n_check
    {
    const std::wregex i18n_review::m_file_filter_regex{ LR"(([*][.][[:alnum:]\*]{1,5}[;]?)+$)" };

    const std::wregex i18n_review::m_url_email_regex{
        LR"(((http|ftp)s?:\/\/)?(www\.)[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))"
    };

    const std::wregex i18n_review::m_malformed_html_tag_bad_amp{ LR"(&amp;[[:alpha:]]{3,5};)" };

    const std::wregex i18n_review::m_malformed_html_tag{ LR"(&(nbsp|amp|quot)[^;])" };

    const std::wregex i18n_review::m_sql_code{
        LR"(.*(SELECT \*|CREATE TABLE|CREATE INDEX|COLLATE NOCASE|ALTER TABLE|DROP TABLE|COLLATE DATABASE_DEFAULT).*)",
        std::regex_constants::icase
    };

    // UINT MENU_ID_PRINT = 1'000;
    const std::wregex i18n_review::m_id_assignment_regex{
        LR"((int|uint32_t|INT|UINT|wxWindowID|#define)([[:space:]]|const)*([a-zA-Z0-9_]*ID[a-zA-Z0-9_]*)[[:space:]]*[=\({]?[[:space:]\({]*([a-zA-Z0-9_ \+\-\'<>:\.]+){1}(.?))"
    };

    const std::wregex i18n_review::m_diagnostic_function_regex{
        LR"(([a-zA-Z0-9_]*|^)(ASSERT|VERIFY|PROFILE|CHECK)([a-zA-Z0-9_]*|$))"
    };

    const std::wregex i18n_review::m_plural_regex{ LR"([[:alnum:]]{2,}[(]s[)])" };
    const std::wregex i18n_review::m_open_function_signature_regex{ LR"([[:alnum:]]{2,}[(])" };
    const std::wregex i18n_review::m_html_tag_regex{ LR"(&[[:alpha:]]{2,5};.*)" };
    const std::wregex i18n_review::m_html_tag_unicode_regex{ LR"(&#[[:digit:]]{2,4};.*)" };
    const std::wregex i18n_review::m_2letter_regex{ LR"([[:alpha:]]{2,})" };
    const std::wregex i18n_review::m_hashtag_regex{ LR"(#[[:alnum:]]{2,})" };
    const std::wregex i18n_review::m_key_shortcut_regex{
        LR"((CTRL|SHIFT|CMD|ALT)([+](CTRL|SHIFT|CMD|ALT))*([+][[:alnum:]])+)",
        std::regex_constants::icase
    };
    const std::wregex i18n_review::m_function_signature_regex{
        LR"([[:alnum:]]{2,}[(][[:alnum:]]+(,[[:space:]]*[[:alnum:]]+)*[)])"
    };

    // HTML, but also includes some GTK formatting tags
    const std::wregex i18n_review::m_html_regex{
        LR"([^[:alnum:]<]*<(span|object|property|div|p|ul|ol|li|img|html|[?]xml|meta|body|table|tbody|tr|td|thead|head|title|a[[:space:]]|!--|/|!DOCTYPE|br|center|dd|em|dl|dt|tt|font|form|h[[:digit:]]|hr|main|map|pre|script).*)",
        std::regex_constants::icase
    };
    // <doc-val>Some text</doc-val>
    const std::wregex i18n_review::m_html_element_with_content_regex{
        LR"(<[a-zA-Z0-9_\-]+>[[:print:][:cntrl:]]*</[a-zA-Z0-9_\-]+>)", std::regex_constants::icase
    };

    // <s:complex name=\"{{GetFunctionName}}_{{GetParameterName}}_Array\">
    const std::wregex i18n_review::m_xml_element_regex{
        LR"(<[/]?[a-zA-Z0-9_:'"\.\[\]\/\{\}\-\\=][a-zA-Z0-9_:'"\.\[\]\/\{\}\- \\=]+[/]?>)",
        std::regex_constants::icase
    };

    // first capture group ensures that printf command is not proceeded by a negating '%'
    // second capture group is the actual printf command
    //
    // Note: a space specifier is OK for numbers, but not strings and pointers:
    //
    // "space: if the result of a signed conversion does not start with a sign character,
    //  or is empty, space is prepended to the result. It is ignored if + flag is present."
    //
    // Note: '<PRId64>' type specifiers are written as part of a printf string
    // (it's a macro outside of the string that the preprocessor maps to something else),
    // but PO files will embed these into the translations and source strings.
    const std::wregex i18n_review::m_printf_cpp_int_regex{
        LR"((^|\b|[%]{2}|[^%])([%]([[:digit:]]+[$])?([+]|[-] #0)?(([*]|[[:digit:]]+)*[.]?[[:digit:]]*)?(l)?(d|i|o|u|zu|c|C|e|E|x|X|l|I|I32|I64|<PRI(d|i|u|x)(32|64)>)))"
    };

    const std::wregex i18n_review::m_printf_cpp_float_regex{
        LR"((^|\b|[%]{2}|[^%])([%]([[:digit:]]+[$])?([+]|[-] #0)?(([*]|[[:digit:]]+)*[.]?[[:digit:]]*)?(l|L)?(f|F)))"
    };

    const std::wregex i18n_review::m_printf_cpp_string_regex{
        LR"((^|\b|[%]{2}|[^%])([%]([[:digit:]]+[$])?[-]?(([*]|[[:digit:]]+)*[.][[:digit:]]*)?s))"
    };

    const std::wregex i18n_review::m_printf_cpp_pointer_regex{
        LR"((^|\b|[%]{2}|[^%])[%]([%]([[:digit:]]+[$])?p))"
    };

    // common font faces that we would usually ignore (client can add to this)
    std::set<string_util::case_insensitive_wstring> i18n_review::m_font_names = { // NOLINT
        L"Arial",
        L"Seaford",
        L"Skeena",
        L"Tenorite",
        L"Courier New",
        L"Garamond",
        L"Calibri",
        L"Gabriola",
        L".Helvetica Neue DeskInterface",
        L".Lucida Grande UI",
        L"Times New Roman",
        L"Georgia",
        L"Segoe UI",
        L"Segoe Script",
        L"Century Gothic",
        L"Century",
        L"Cascadia Mono",
        L"URW Bookman L",
        L"AR Berkley",
        L"Brush Script",
        L"Consolas",
        L"Century Schoolbook L",
        L"Lucida Grande",
        L"Helvetica Neue",
        L"Liberation Serif",
        L"Luxi Serif",
        L"Ms Shell Dlg",
        L"Ms Shell Dlg 2",
        L"Bitstream Vera Serif",
        L"URW Palladio L",
        L"URW Chancery L",
        L"Comic Sans MS",
        L"DejaVu Serif",
        L"DejaVu LGC Serif",
        L"Nimbus Sans L",
        L"URW Gothic L",
        L"Lucida Sans",
        L"Andale Mono",
        L"Luxi Sans",
        L"Liberation Sans",
        L"Bitstream Vera Sans",
        L"DejaVu LGC Sans",
        L"DejaVu Sans",
        L"Nimbus Mono L",
        L"Lucida Sans Typewriter",
        L"Luxi Mono",
        L"DejaVu Sans Mono",
        L"DejaVu LGC Sans Mono",
        L"Bitstream Vera Sans Mono",
        L"Liberation Mono",
        L"Franklin Gothic",
        L"Aptos",
        L"Grandview",
        L"Bierstadt"
    };

    // documents
    std::set<string_util::case_insensitive_wstring> i18n_review::m_file_extensions = { // NOLINT
        L"xml", L"html", L"htm", L"xhtml", L"rtf", L"doc", L"docx", L"dot", L"docm", L"txt", L"ppt",
        L"pptx", L"pdf", L"ps", L"odt", L"ott", L"odp", L"otp", L"pptm", L"md", L"xaml",
        // Visual Studio files
        L"sln", L"csproj", L"json", L"pbxproj", L"apk", L"tlb", L"ocx", L"pdb", L"tlh", L"hlp",
        L"msi", L"rc", L"vcxproj", L"resx", L"appx", L"vcproj",
        // macOS
        L"dmg", L"proj", L"xbuild", L"xmlns",
        // Database
        L"mdb", L"db",
        // Markdown files
        L"md", L"Rmd", L"qmd", L"yml",
        // help files
        L"hhc", L"hhk", L"hhp",
        // spreadsheets
        L"xls", L"xlsx", L"ods", L"csv",
        // image formats
        L"gif", L"jpg", L"jpeg", L"jpe", L"bmp", L"tiff", L"tif", L"png", L"tga", L"svg", L"xcf",
        L"ico", L"psd", L"hdr", L"pcx",
        // webpages
        L"asp", L"aspx", L"cfm", L"cfml", L"php", L"php3", L"php4", L"sgml", L"wmf", L"js",
        // style sheets
        L"css",
        // movies
        L"mov", L"qt", L"rv", L"rm", L"wmv", L"mpg", L"mpeg", L"mpe", L"avi",
        // music
        L"mp3", L"wav", L"wma", L"midi", L"ra", L"ram",
        // programs
        L"exe", L"swf", L"vbs",
        // source files
        L"cpp", L"h", L"c", L"idl", L"cs", L"hpp", L"po",
        // compressed files
        L"gzip", L"bz2"
    };

    std::set<std::wstring> i18n_review::m_untranslatable_exceptions = { L"PhD" };

    std::vector<std::wregex> i18n_review::m_variable_name_patterns_to_ignore;

    // variables whose CTORs take a string that should never be translated
    std::set<std::wstring> i18n_review::m_variable_types_to_ignore = {
        L"wxUxThemeHandle",
        L"wxRegKey",
        L"wxXmlNode",
        L"wxLoadedDLL",
        L"wxConfigPathChanger",
        L"wxWebViewEvent",
        L"wxFileSystemWatcherEvent",
        L"wxStdioPipe",
        L"wxCMD_LINE_CHARS_ALLOWED_BY_SHORT_OPTION",
        L"vmsWarningHandler",
        L"vmsErrorHandler",
        L"wxFFileOutputStream",
        L"wxFFile",
        L"wxFileName",
        L"wxColor",
        L"wxColour",
        L"wxFont",
        L"LOGFONTW",
        L"SecretSchema",
        L"GtkTypeInfo",
        L"QKeySequence",
        L"wxRegEx",
        L"wregex",
        L"std::wregex",
        L"regex",
        L"std::regex",
        L"ifstream",
        L"ofstream",
        L"FileStream",
        L"StreamWriter",
        L"CultureInfo",
        L"TagHelperAttribute",
        L"QRegularExpression",
        L"wxDataViewRenderer",
        L"wxDataViewBitmapRenderer",
        L"wxDataViewDateRenderer",
        L"wxDataViewTextRenderer",
        L"wxDataViewIconTextRenderer",
        L"wxDataViewCustomRenderer",
        L"wxDataViewToggleRenderer",
        L"wxDataObjectSimple"
    };

    //--------------------------------------------------
    i18n_review::i18n_review()
        {
        m_deprecated_string_macros = {
            { L"wxT", _WXTRANS_WSTR(L"wxT() macro can be removed.") },
            { L"wxT_2", _WXTRANS_WSTR(L"wxT_2() macro can be removed.") },
            // wxWidgets can convert ANSI strings to double-byte, but Win32/MFC can't
            // and will need an 'L' prefixed to properly replace _T like macros.
            { L"_T",
              _WXTRANS_WSTR(L"_T() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"__T",
              _WXTRANS_WSTR(L"__T() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"TEXT",
              _WXTRANS_WSTR(L"TEXT() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"_TEXT",
              _WXTRANS_WSTR(
                  L"_TEXT() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"__TEXT",
              _WXTRANS_WSTR(
                  L"__TEXT() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"_WIDE",
              _WXTRANS_WSTR(
                  L"_WIDE() macro can be removed. Prefix with 'L' to make string wide.") }
        };

        // Whole file needs to be scanned for these, as string variables can be passed to these
        // as well as hard-coded strings.
        m_deprecated_string_functions = {
            // Win32 TCHAR functions (which mapped between _MBCS and _UNICODE builds).
            // Nowadays, you should always be compiling as _UNICODE (i.e., UTF-16).
            { L"__targv", _WXTRANS_WSTR(L"Use __wargv instead of __targv.") },
            { L"__tcserror", _WXTRANS_WSTR(L"Use __wcserror() instead of __tcserror().") },
            { L"__tcserror_s", _WXTRANS_WSTR(L"Use __wcserror_s() instead of __tcserror_s().") },
            { L"_tcscat", _WXTRANS_WSTR(L"Use std::wcscat() instead of _tcscat().") },
            { L"_tcscat_s", _WXTRANS_WSTR(L"Use wcscat_s() instead of _tcscat_s().") },
            { L"_tcschr", _WXTRANS_WSTR(L"Use std::wcschr() instead of _tcschr().") },
            { L"_tcsclen", _WXTRANS_WSTR(L"Use std::wcslen() instead of _tcsclen().") },
            { L"_tcscmp", _WXTRANS_WSTR(L"Use std::wcscmp() instead of _tcscmp().") },
            { L"_tcscnlen", _WXTRANS_WSTR(L"Use std::wcsnlen() instead of _tcscnlen().") },
            { L"_tcscoll", _WXTRANS_WSTR(L"Use std::wcscoll() instead of _tcscoll().") },
            { L"_tcscoll_l", _WXTRANS_WSTR(L"Use _wcscoll_l() instead of _tcscoll_l().") },
            { L"_tcscpy", _WXTRANS_WSTR(L"Use std::wcscpy() instead of _tcscpy().") },
            { L"_tcscpy_s", _WXTRANS_WSTR(L"Use wcscpy_s() instead of _tcscpy_s().") },
            { L"_tcscspn", _WXTRANS_WSTR(L"Use std::wcscspn() instead of _tcscspn().") },
            { L"_tcsdup", _WXTRANS_WSTR(L"Use _wcsdup() instead of _tcsdup().") },
            { L"_tcserror", _WXTRANS_WSTR(L"Use _wcserror() instead of _tcserror().") },
            { L"_tcserror_s", _WXTRANS_WSTR(L"Use _wcserror_s() instead of _tcserror_s().") },
            { L"_tcsicmp", _WXTRANS_WSTR(L"Use _wcsicmp() instead of _tcsicmp().") },
            { L"_tcsicmp_l", _WXTRANS_WSTR(L"Use _wcsicmp_l() instead of _tcsicmp_l().") },
            { L"_tcsicoll", _WXTRANS_WSTR(L"Use _wcsicoll() instead of _tcsicoll().") },
            { L"_tcsicoll_l", _WXTRANS_WSTR(L"Use _wcsicoll_l() instead of _tcsicoll_l().") },
            { L"_tcslen", _WXTRANS_WSTR(L"Use std::wcslen() instead of _tcslen().") },
            { L"_tcsncat", _WXTRANS_WSTR(L"Use std::wcsncat() instead of _tcsncat().") },
            { L"_tcsncat_l", _WXTRANS_WSTR(L"Use _wcsncat_l() instead of _tcsncat_l().") },
            { L"_tcsncat_s", _WXTRANS_WSTR(L"Use std::wcsncat_s() instead of _tcsncat_s().") },
            { L"_tcsncat_s_l", _WXTRANS_WSTR(L"Use _wcsncat_s_l() instead of _tcsncat_s_l().") },
            { L"_tcsnccmp", _WXTRANS_WSTR(L"Use std::wcsncmp() instead of _tcsnccmp().") },
            { L"_tcsnccoll", _WXTRANS_WSTR(L"Use _wcsncoll() instead of _tcsnccoll().") },
            { L"_tcsnccoll_l", _WXTRANS_WSTR(L"Use _wcsncoll_l() instead of _tcsnccoll_l().") },
            { L"_tcsncicmp", _WXTRANS_WSTR(L"Use _wcsnicmp() instead of _tcsncicmp().") },
            { L"_tcsncicmp_l", _WXTRANS_WSTR(L"Use _wcsnicmp_l() instead of _tcsncicmp_l().") },
            { L"_tcsncicoll", _WXTRANS_WSTR(L"Use _wcsnicoll() instead of _tcsncicoll().") },
            { L"_tcsncicoll_l", _WXTRANS_WSTR(L"Use _wcsnicoll_l() instead of _tcsncicoll_l().") },
            { L"_tcsncmp", _WXTRANS_WSTR(L"Use std::wcsncmp() instead of _tcsncmp().") },
            { L"_tcsncoll", _WXTRANS_WSTR(L"Use _wcsncoll() instead of _tcsncoll().") },
            { L"_tcsncoll_l", _WXTRANS_WSTR(L"Use _wcsncoll_l() instead of _tcsncoll_l().") },
            { L"_tcsncpy", _WXTRANS_WSTR(L"Use std::wcsncpy() instead of _tcsncpy().") },
            { L"_tcsncpy_l", _WXTRANS_WSTR(L"Use _wcsncpy_l() instead of _tcsncpy_l().") },
            { L"_tcsncpy_s", _WXTRANS_WSTR(L"Use wcsncpy_s() instead of _tcsncpy_s().") },
            { L"_tcsncpy_s_l", _WXTRANS_WSTR(L"Use _wcsncpy_s_l() instead of _tcsncpy_s_l().") },
            { L"_tcsnicmp", _WXTRANS_WSTR(L"Use _wcsnicmp() instead of _tcsnicmp().") },
            { L"_tcsnicmp_l", _WXTRANS_WSTR(L"Use _wcsnicmp_l() instead of _tcsnicmp_l().") },
            { L"_tcsnicoll", _WXTRANS_WSTR(L"Use _wcsnicoll() instead of _tcsnicoll().") },
            { L"_tcsnicoll_l", _WXTRANS_WSTR(L"Use _wcsnicoll_l() instead of _tcsnicoll_l().") },
            { L"_tcsnlen", _WXTRANS_WSTR(L"Use std::wcsnlen() instead of _tcsnlen().") },
            { L"_tcsnset", _WXTRANS_WSTR(L"Use _wcsnset() instead of _tcsnset().") },
            { L"_tcsnset_l", _WXTRANS_WSTR(L"Use _wcsnset_l() instead of _tcsnset_l().") },
            { L"_tcsnset_s", _WXTRANS_WSTR(L"Use _wcsnset_s() instead of _tcsnset_s().") },
            { L"_tcsnset_s_l", _WXTRANS_WSTR(L"Use _wcsnset_s_l() instead of _tcsnset_s_l().") },
            { L"_tcspbrk", _WXTRANS_WSTR(L"Use std::wcspbrk() instead of _tcspbrk().") },
            { L"_tcsrchr", _WXTRANS_WSTR(L"Use std::wcsrchr() instead of _tcsrchr().") },
            { L"_tcsrev", _WXTRANS_WSTR(L"Use _wcsrev() instead of _tcsrev().") },
            { L"_tcsset", _WXTRANS_WSTR(L"Use _wcsset() instead of _tcsset().") },
            { L"_tcsset_l", _WXTRANS_WSTR(L"Use _wcsset_l() instead of _tcsset_l().") },
            { L"_tcsset_s", _WXTRANS_WSTR(L"Use _wcsset_s() instead of _tcsset_s().") },
            { L"_tcsset_s_l", _WXTRANS_WSTR(L"Use _wcsset_s_l() instead of _tcsset_s_l().") },
            { L"_tcsspn", _WXTRANS_WSTR(L"Use std::wcsspn() instead of _tcsspn().") },
            { L"_tcsstr", _WXTRANS_WSTR(L"Use std::wcsstr() instead of _tcsstr().") },
            { L"_tcstod", _WXTRANS_WSTR(L"Use std::wcstod() instead of _tcstod().") },
            { L"_tcstof", _WXTRANS_WSTR(L"Use std::wcstof() instead of _tcstof().") },
            { L"_tcstoimax", _WXTRANS_WSTR(L"Use std::wcstoimax() instead of _tcstoimax().") },
            { L"_tcstok", _WXTRANS_WSTR(L"Use _wcstok() instead of _tcstok().") },
            { L"_tcstok_l", _WXTRANS_WSTR(L"Use _wcstok_l() instead of _tcstok_l().") },
            { L"_tcstok_s", _WXTRANS_WSTR(L"Use wcstok_s() instead of _tcstok_s().") },
            { L"_tcstok_s_l", _WXTRANS_WSTR(L"Use _wcstok_s_l() instead of _tcstok_s_l().") },
            { L"_tcstol", _WXTRANS_WSTR(L"Use std::wcstol() instead of _tcstol().") },
            { L"_tcstold", _WXTRANS_WSTR(L"Use std::wcstold() instead of _tcstold().") },
            { L"_tcstoll", _WXTRANS_WSTR(L"Use std::wcstoll() instead of _tcstoll().") },
            { L"_tcstoul", _WXTRANS_WSTR(L"Use std::wcstoul() instead of _tcstoul().") },
            { L"_tcstoull", _WXTRANS_WSTR(L"Use std::wcstoull() instead of _tcstoull().") },
            { L"_tcstoumax", _WXTRANS_WSTR(L"Use std::wcstoumax() instead of _tcstoumax().") },
            { L"_tcsxfrm", _WXTRANS_WSTR(L"Use std::wcsxfrm() instead of _tcsxfrm()") },
            { L"_tenviron", _WXTRANS_WSTR(L"Use _wenviron() instead of _tenviron().") },
            { L"_tmain", _WXTRANS_WSTR(L"Use wmain() instead of _tmain().") },
            { L"_tprintf", _WXTRANS_WSTR(L"Use wprintf() instead of _tprintf().") },
            { L"_tprintf_l", _WXTRANS_WSTR(L"Use _wprintf_l() instead of _tprintf_l().") },
            { L"_tprintf_s", _WXTRANS_WSTR(L"Use wprintf_s() instead of _tprintf_s().") },
            { L"_tWinMain", _WXTRANS_WSTR(L"Use wWinMain() instead of _tWinMain().") },
            { L"wsprintf", _WXTRANS_WSTR(L"Use std::swprintf() instead of wsprintf().") },
            { L"_stprintf", _WXTRANS_WSTR(L"Use std::swprintf() instead of _stprintf().") },
            { L"TCHAR", _WXTRANS_WSTR(L"Use wchar_t instead of TCHAR.") },
            { L"PTCHAR", _WXTRANS_WSTR(L"Use wchar_t* instead of PTCHAR.") },
            { L"LPTSTR", _WXTRANS_WSTR(L"Use LPWSTR (or wchar_t*) instead of LPTSTR.") },
            { L"LPCTSTR", _WXTRANS_WSTR(L"Use LPCWSTR (or const wchar_t*) instead of LPCTSTR.") },
            // wxWidgets
            { L"wxStrlen",
              _WXTRANS_WSTR(
                  L"Use std::wcslen() or (wrap in a std::wstring_view) instead of wxStrlen().") },
            { L"wxStrstr", _WXTRANS_WSTR(L"Use std::wcsstr() instead of wxStrstr().") },
            { L"wxStrchr", _WXTRANS_WSTR(L"Use std::wcschr() instead of wxStrchr().") },
            { L"wxStrdup", _WXTRANS_WSTR(L"Use std::wcsdup() instead of wxStrdup().") },
            { L"wxStrcpy",
              _WXTRANS_WSTR(L"Use std::wcscpy() instead of wxStrcpy() "
                            "(or prefer safer functions that process N number of characters).") },
            { L"wxStrncpy",
              _WXTRANS_WSTR(L"Use std::wcsncpy() (or wxStrlcpy) instead of wxStrncpy().") },
            { L"wxStrcat ",
              _WXTRANS_WSTR(L"Use std::wcscat() instead of wxStrcat() "
                            "(or prefer safer functions that process N number of characters).") },
            { L"wxStrncat", _WXTRANS_WSTR(L"Use std::wcsncat() instead of wxStrncat().") },
            { L"wxStrtok", _WXTRANS_WSTR(L"Use std::wcstok() instead of wxStrtok().") },
            { L"wxStrrchr", _WXTRANS_WSTR(L"Use std::wcsrchr() instead of wxStrrchr().") },
            { L"wxStrpbrk", _WXTRANS_WSTR(L"Use std::wcspbrk() instead of wxStrpbrk().") },
            { L"wxStrxfrm", _WXTRANS_WSTR(L"Use std::wcsxfrm() instead of wxStrxfrm.") },
            { L"wxIsEmpty",
              _WXTRANS_WSTR(L"Use wxString's empty() member instead of wxIsEmpty().") },
            { L"wxIsdigit", _WXTRANS_WSTR(L"Use std::iswdigit() instead of wxIsdigit().") },
            { L"wxIsalnum", _WXTRANS_WSTR(L"Use std::iswalnum() instead of wxIsalnum().") },
            { L"wxIsalpha", _WXTRANS_WSTR(L"Use std::iswalpha() instead of wxIsalpha().") },
            { L"wxIsctrl", _WXTRANS_WSTR(L"Use std::iswctrl() instead of wxIsctrl().") },
            { L"wxIspunct", _WXTRANS_WSTR(L"Use std::iswpunct() instead of wxIspunct().") },
            { L"wxIsspace", _WXTRANS_WSTR(L"Use std::iswpspace() instead of wxIsspace().") },
            { L"wxChar", _WXTRANS_WSTR(L"Use wchar_t instead of wxChar.") },
            { L"wxSChar", _WXTRANS_WSTR(L"Use wchar_t instead of wxSChar.") },
            { L"wxUChar", _WXTRANS_WSTR(L"Use wchar_t instead of wxUChar.") },
            { L"wxStrftime",
              _WXTRANS_WSTR(L"Use wxDateTime's formatting functions instead of wxStrftime().") },
            { L"wxStrtod", _WXTRANS_WSTR(L"Use wxString::ToDouble() instead of wxStrtod.") },
            { L"wxStrtol", _WXTRANS_WSTR(L"Use wxString::ToLong() instead of wxStrtol.") },
            { L"wxW64", _WXTRANS_WSTR(L"wxW64 is obsolete; remove it.") },
            { L"__WXFUNCTION__",
              _WXTRANS_WSTR(L"Use __func__ or __WXFUNCTION_SIG__ (requires wxWidgets 3.3) "
                            "instead of __WXFUNCTION__.") },
            { L"wxTrace",
              _WXTRANS_WSTR(L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                            L"functions instead of wxTrace.") },
            { L"WXTRACE",
              _WXTRANS_WSTR(L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                            L"functions instead of WXTRACE.") },
            { L"wxTraceLevel",
              _WXTRANS_WSTR(L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                            L"functions instead of wxTraceLevel.") },
            { L"wxUnix2DosFilename",
              _WXTRANS_WSTR(
                  L"Construct a wxFileName with wxPATH_UNIX and then use "
                  "wxFileName::GetFullPath(wxPATH_DOS) instead of using wxUnix2DosFilename.") },
            { L"wxSplitPath",
              _WXTRANS_WSTR(
                  L"wxSplitPath is obsolete, please use wxFileName::SplitPath() instead.") },
            // not i18n related, just legacy wx functions that can be modernized
            { L"wxMin", _WXTRANS_WSTR(L"Use std::min() instead of wxMin().") },
            { L"wxMax", _WXTRANS_WSTR(L"Use std::max() instead of wxMax().") },
            { L"wxRound", _WXTRANS_WSTR(L"Use std::lround() instead of wxRound().") },
            { L"wxIsNan", _WXTRANS_WSTR(L"Use std::isnan() instead of wxIsNan().") },
            { L"wxNOEXCEPT", _WXTRANS_WSTR(L"Use noexcept instead of wxNOEXCEPT.") },
            { L"__WXMAC__", _WXTRANS_WSTR(L"Use __WXOSX__ instead of __WXMAC__.") },
            { L"wxMEMBER_DELETE", _WXTRANS_WSTR(L"Use '= delete' instead of wxMEMBER_DELETE.") },
            { L"wxOVERRIDE", _WXTRANS_WSTR(L"Use override or final instead of wxOVERRIDE.") },
            { L"wxDECLARE_NO_COPY_CLASS",
              _WXTRANS_WSTR(L"Delete the copy CTOR and assignment operator instead of "
                            "wxDECLARE_NO_COPY_CLASS.") },
            { L"DECLARE_NO_COPY_CLASS",
              _WXTRANS_WSTR(L"Delete the copy CTOR and assignment operator "
                            "instead of DECLARE_NO_COPY_CLASS.") },
            { L"wxGROW",
              _WXTRANS_WSTR(L"Call wxSizer::Add() with a wxSizerFlags object using Expand() "
                            "instead of wxGROW.") },
            { L"wxEXPAND",
              _WXTRANS_WSTR(L"Call wxSizer::Add() with a wxSizerFlags object using Expand() "
                            "instead of wxEXPAND.") }
        };

        if (m_min_cpp_version >= 2017)
            {
            m_deprecated_string_functions.insert(
                { L"wxNODISCARD", _WXTRANS_WSTR(L"Use [[nodiscard]] instead of wxNODISCARD.") });
            m_deprecated_string_functions.insert(
                { L"WXSIZEOF", _WXTRANS_WSTR(L"Use std::size() instead of WXSIZEOF().") });
            m_deprecated_string_functions.insert(
                { L"wxUnusedVar", _WXTRANS_WSTR(L"Use [[maybe_unused]] instead of wxUnusedVar.") });
            m_deprecated_string_functions.insert(
                { L"WXUNUSED", _WXTRANS_WSTR(L"Use [[maybe_unused]] instead of WXUNUSED().") });
            }

        m_translatable_regexes = { std::wregex(LR"(Q[0-9](F|A)Y)") };

        m_untranslatable_regexes = {
            // nothing but numbers, punctuation, or control characters?
            std::wregex(LR"(([[:digit:][:space:][:punct:][:cntrl:]]|\\[rnt])+)"),
            // placeholder text
            std::wregex(LR"(Lorem ipsum.*)"),
            // webpage content type
            std::wregex(
                LR"([A-Za-z0-9\-]+/[A-Za-z0-9\-]+;[[:space:]]*[A-Za-z0-9\-]+=[A-Za-z0-9\-]+)"),
            // SQL code
            m_sql_code,
            std::wregex(LR"(^(INSERT INTO|DELETE FROM).*)", std::regex_constants::icase),
            std::wregex(LR"(^ORDER BY.*)"), // more strict
            std::wregex(LR"([(]*^SELECT[[:space:]]+[A-Z_0-9\.]+,.*)"),
            // a regex expression
            std::wregex(LR"([(][?]i[)].*)"),
            // single file filter that just has a file extension as its "name"
            // PNG (*.png)
            // PNG (*.png)|*.png
            // TIFF (*.tif;*.tiff)|*.tif;*.tiff
            // special case for the word "bitmap" also, wouldn't normally translate that
            std::wregex(LR"(([A-Z]+|[bB]itmap) [(]([*][.][A-Za-z0-9]{1,7}[)]))"),
            std::wregex(
                LR"((([A-Z]+|[bB]itmap) [(]([*][.][A-Za-z0-9]{1,7})(;[*][.][A-Za-z0-9]{1,7})*[)][|]([*][.][A-Za-z0-9]{1,7})(;[*][.][A-Za-z0-9]{1,7})*[|]{0,2})+)"),
            // multiple file filters next to each other
            std::wregex(LR"(([*][.][A-Za-z0-9]{1,7}[;]?[[:space:]]*)+)"),
            // clang-tidy commands
            std::wregex(LR"(\-checks=.*)"),
            // generic measuring string (or regex expression)
            std::wregex(LR"([[:space:]]*(ABCDEFG|abcdefg|AEIOU|aeiou).*)"),
            // debug messages
            std::wregex(LR"(Assert(ion)? (f|F)ail.*)"), std::wregex(LR"(ASSERT *)"),
            // HTML doc start
            std::wregex(LR"(<!DOCTYPE html)"),
            // HTML entities
            std::wregex(LR"(&[#]?[xX]?[A-Za-z0-9]+;)"), std::wregex(LR"(<a href=.*)"),
            // CSS
            std::wregex(
                LR"([\s\S]*(\{[[:space:]]*[a-zA-Z\-]+[[:space:]]*[:][[:space:]]*[0-9a-zA-Z\- \(\);\:%#'",]+[[:space:]]*\})+[\s\S]*)"),
            // JS
            std::wregex(LR"(class[[:space:]]*=[[:space:]]*['"][A-Za-z0-9\- _]*['"])"),
            // An opening HTML element
            std::wregex(LR"(<(body|html|img|head|meta|style|span|p|tr|td))"),
            // PostScript element
            std::wregex(LR"(%%[[:alpha:]]+:.*)"),
            std::wregex(LR"((<< [\/()A-Za-z0-9[:space:]]*(\\n|[[:space:]])*)+)"),
            std::wregex(
                LR"((\/[A-Za-z0-9[:space:]]* \[[A-Za-z0-9[:space:]%]+\](\\n|[[:space:]])*)+)"),
            // C
            std::wregex(
                LR"(^#(include|define|if|ifdef|ifndef|endif|elif|pragma|warning)[[:space:]].*)"),
            // C++
            std::wregex(LR"([a-zA-Z0-9_]+([-][>]|::)[a-zA-Z0-9_]+([(][)];)?)"),
            // XML elements
            std::wregex(LR"(version[ ]?=\\"[0-9\.]+\\")"),
            std::wregex(LR"(<([A-Za-z])+([A-Za-z0-9_/\\\-\.'"=;:#[:space:]])+[>]?)"),
            std::wregex(LR"(xml[ ]*version[ ]*=[ ]*\\["'][0-9\.]+\\["'][>]?)"), // partial header
            std::wregex(LR"(<[\\]?\?xml[ a-zA-Z0-9=\\"'%\-]*[\?]?>)"),          // full header
            std::wregex(
                LR"(<[A-Za-z]+[A-Za-z0-9_/\\\-\.'"=;:[:space:]]+>[[:space:][:digit:][:punct:]]*<[A-Za-z0-9_/\-.']*>)"),
            std::wregex(LR"(<[A-Za-z]+([A-Za-z0-9_\-\.]+[[:space:]]*){1,2}=[[:punct:]A-Za-z0-9]*)"),
            std::wregex(LR"(^[[:space:]]*xmlns(:[[:alnum:]]+)?=.*)"),
            std::wregex(LR"(^[[:space:]]*<soap:[[:alnum:]]+.*)"),
            std::wregex(LR"(^[[:space:]]*<port\b.*)"),
            std::wregex(LR"(^\{\{.*)"), // soap syntax
            // <image x=%d y=\"%d\" width = '%dpx' height="%dpx"
            std::wregex(
                LR"(<[A-Za-z0-9_\-\.]+[[:space:]]*([A-Za-z0-9_\-\.]+[[:space:]]*=[[:space:]]*[\"'\\]{0,2}[a-zA-Z0-9\-]*[\"'\\]{0,2}[[:space:]]*)+)"),
            std::wregex(L"charset[[:space:]]*=.*", std::regex_constants::icase),
            // all 'X'es, spaces, and commas are usually a placeholder of some sort
            std::wregex(LR"((([\+\-]?[xX\.]+)[ ,]*)+)"),
            // program version string
            std::wregex(LR"([a-zA-Z\-]+ v(ersion)?[ ]?[0-9\.]+)"),
            // bash command (e.g., "lpstat -p") and system variables
            std::wregex(LR"([[:alpha:]]{3,} [\-][[:alpha:]]+)"), std::wregex(LR"(sys[$].*)"),
            // Pascal-case words (e.g., "GetValueFromUser");
            // surrounding punctuation is stripped first.
            std::wregex(LR"([[:punct:]]*[A-Z]+[a-z0-9]+([A-Z]+[a-z0-9]+)+[[:punct:]]*)"),
            // camel-case words (e.g., "getValueFromUser", "unencodedExtASCII");
            // surrounding punctuation is stripped first.
            std::wregex(LR"([[:punct:]]*[a-z]+[[:digit:]]*([A-Z]+[a-z0-9]*)+[[:punct:]]*)"),
            // reverse camel-case (e.g., "UTF8FileWithBOM")
            std::wregex(LR"([[:punct:]]*[A-Z]+[[:digit:]]*([a-z0-9]+[A-Z]+)+[[:punct:]]*)"),
            // formulas (e.g., ABS(-2.7), POW(-4, 2), =SUM(1; 2) )
            std::wregex(LR"((=)?[A-Za-z0-9_]{3,}[(]([RC0-9\-\.,;:\[\] ])*[)])"),
            // formulas (e.g., ComputeNumbers() )
            std::wregex(LR"([A-Za-z0-9_]{3,}[(][)])"),
            // equal sign followed by a single word is probably some sort of
            // config file tag or formula.
            std::wregex(LR"(=[A-Za-z0-9_]+)"),
            // character encodings
            std::wregex(
                LR"((utf[-]?[[:digit:]]+|Shift[-_]JIS|us-ascii|windows-[[:digit:]]{4}|KOI8-R|Big5|GB2312|iso-[[:digit:]]{4}-[[:digit:]]+))",
                std::regex_constants::icase),
            // wxWidgets constants
            std::wregex(LR"((wx|WX)[A-Z_0-9]{2,})"),
            // ODCTask --surrounding punctuation is stripped first
            std::wregex(LR"([[:punct:]]*[A-Z]{3,}[a-z_0-9]{2,}[[:punct:]]*)"),
            // snake case words
            // Note that "P_rinter" would be OK, as the '_' may be a hot-key accelerator
            std::wregex(LR"([_]*[a-z0-9]+(_[a-z0-9]+)+[_]*)"), // user_level_permission
            std::wregex(LR"([_]*[A-Z0-9]+(_[A-Z0-9]+)+[_]*)"), // __HIGH_SCORE__
            std::wregex(LR"([_]*[A-Z0-9][a-z0-9]+(_[A-Z0-9][a-z0-9]+)+[_]*)"), // Config_File_Path
            // CSS strings
            std::wregex(
                LR"(font-(style|weight|family|size|face-name|underline|point-size)[[:space:]]*[:]?.*)",
                std::regex_constants::icase),
            std::wregex(LR"(text-decoration[[:space:]]*[:]?.*)", std::regex_constants::icase),
            std::wregex(LR"((background-)?color[[:space:]]*:.*)", std::regex_constants::icase),
            std::wregex(LR"(style[[:space:]]*=["']?.*)", std::regex_constants::icase),
            // local file paths & file names
            std::wregex(LR"((WINDIR|Win32|System32|Kernel32|/etc|/tmp))",
                        std::regex_constants::icase),
            std::wregex(LR"((so|dll|exe|dylib|jpg|bmp|png|gif|txt|doc))",
                        std::regex_constants::icase), // common file extension that might be missing
                                                      // the period
            std::wregex(LR"([.][a-zA-Z0-9]{1,5})"),   // file extension
            std::wregex(LR"([.]DS_Store)"),           // macOS file
            // file name (supports multiple extensions)
            std::wregex(LR"([\\/]?[[:alnum:]_~!@#$%&;',+={}().^\[\]\-]+([.][a-zA-Z0-9]{1,4})+)"),
            // ultra simple relative file path (e.g., "shaders/player1.vert")
            std::wregex(LR"(([[:alnum:]_-]+[\\/]){1,2}[[:alnum:]_-]+([.][a-zA-Z0-9]{1,4})+)"),
            std::wregex(LR"(\*[.][a-zA-Z0-9]{1,5})"), // wild card file extension
            // UNIX or web folder (needs at least 1 folder in path)
            std::wregex(LR"(([/]{1,2}[[:alnum:]_~!@#$%&;',+={}().^\[\]\-]+){2,}/?)"),
            // Windows folder
            std::wregex(LR"([a-zA-Z][:]([\\]{1,2}[[:alnum:]_~!@#$%&;',+={}().^\[\]\-]*)+)"),
            std::wregex(LR"([/]?sys\$.*)"),
            // Debug message
            std::wregex(LR"(^DEBUG:[\s\S].*)"),
            // mail protocols
            std::wregex(LR"(^(RCPT TO|MAIL TO|MAIL FROM):.*)"),
            // GUIDs
            std::wregex(
                LR"(^\{[a-fA-F0-9]{8}\-[a-fA-F0-9]{4}\-[a-fA-F0-9]{4}\-[a-fA-F0-9]{4}\-[a-fA-F0-9]{12}\}$)"),
            // encoding
            std::wregex(LR"(^(base[0-9]+|uuencode|quoted-printable)$)"),
            std::wregex(LR"(^(250\-AUTH)$)"),
            // MIME headers
            std::wregex(LR"(^MIME-Version:.*)"), std::wregex(LR"(^X-Priority:.*)"),
            std::wregex(LR"(^(application/octet-stream|text/plain|rawdata)$)"),
            std::wregex(LR"(.*\bContent-Type:[[:space:]]*[[:alnum:]]+/[[:alnum:]]+;.*)"),
            std::wregex(LR"(.*\bContent-Transfer-Encoding:[[:space:]]*[[:alnum:]]+.*)"),
            // URL
            std::wregex(
                LR"(((http|ftp)s?:\/\/)?(www\.)[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))"),
            // email address
            std::wregex(
                LR"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$)"),
            std::wregex(
                LR"(^[\w ]*<[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*>$)"),
            // Windows HTML clipboard data
            std::wregex(LR"(.*(End|Start)(HTML|Fragment)[:]?[[:digit:]]*.*)"),
            // printer commands (e.g., @PAGECOUNT@)
            std::wregex(LR"(@[A-Z0-9]+@)"),
            // [CMD]
            std::wregex(LR"(\[[A-Z0-9]+\])"),
            // Windows OS names
            std::wregex(
                LR"((Microsoft )?Windows (95|98|NT|ME|2000|Server|Vista|Longhorn|XP|[[:digit:]]{1,2}[.]?[[:digit:]]{0,2})[[:space:]]*[[:digit:]]{0,4}[[:space:]]*(R|SP)?[[:digit:]]{0,2})"),
            // products and standards
            std::wregex(LR"((Misra|MISRA) C( [0-9]+)?)"),
            std::wregex(LR"(Borland C\+\+ Builder( [0-9]+)?)"), std::wregex(LR"(Qt Creator)"),
            std::wregex(LR"((Microsoft )VS Code)"), std::wregex(LR"((Microsoft )?Visual Studio)"),
            std::wregex(LR"((Microsoft )?Visual C\+\+)"),
            std::wregex(LR"((Microsoft )?Visual Basic)"),
            // culture language tags
            std::wregex(LR"([a-z]{2,3}[\-_][A-Z]{2,3})"),
            // image formats
            std::wregex(LR"(TARGA|PNG|JPEG|JPG|BMP|GIF)")
        };

        // functions/macros that indicate that a string will be localizable
        m_localization_functions = {
            // GNU's gettext C/C++ functions
            L"_", L"gettext", L"dgettext", L"ngettext", L"dngettext", L"pgettext", L"dpgettext",
            L"npgettext", L"dnpgettext", L"dcgettext",
            // GNU's propername module
            L"proper_name", L"proper_name_utf8",
            // wxWidgets gettext wrapper functions
            L"wxPLURAL", L"wxGETTEXT_IN_CONTEXT", L"wxGETTEXT_IN_CONTEXT_PLURAL", L"wxTRANSLATE",
            L"wxTRANSLATE_IN_CONTEXT", L"wxGetTranslation",
            // Qt (note that NOOP functions actually do load something for translation,
            // just not in-place)
            L"tr", L"trUtf8", L"translate", L"QT_TR_NOOP", L"QT_TRANSLATE_NOOP",
            L"QApplication::translate", L"QApplication::tr", L"QApplication::trUtf8",
            // KDE (ki18n)
            L"i18n", L"i18np", L"i18ncp", L"i18nc", L"xi18n", L"xi18nc",
            // our own functions
            L"_WXTRANS_WSTR"
        };

        // functions that indicate that a string is explicitly marked to not be translatable
        m_non_localizable_functions = {
            L"_DT", L"DONTTRANSLATE",
            // these are not defined explicitly in gettext, but their documentation suggests
            // that you can add them as defines in your code and use them
            L"gettext_noop", L"N_"
        };

        // Constructors and macros that should be ignored
        // (when backtracing, these are skipped over, and the parser moves to the
        //  function/variable assignment to the left of these).
        m_ctors_to_ignore = {
            // Win32 text macros that should be skipped over
            L"_T", L"TEXT", L"_TEXT", L"__TEXT", L"_WIDE",
            // macOS
            L"CFSTR", L"CFStringRef",
            // similar macros from other libraries
            L"T",
            // wxWidgets
            L"wxT", L"wxT_2", L"wxS", L"wxString", L"wxBasicString", L"wxCFStringRef",
            L"wxASCII_STR", L"wxFile",
            // Qt
            L"QString", L"QLatin1String", L"QStringLiteral", L"setStyleSheet", L"QFile",
            // standard string objects
            L"basic_string", L"string", L"wstring", L"u8string", L"u16string", L"u32string",
            L"std::basic_string", L"std::string", L"std::wstring", L"std::u8string",
            L"std::u16string", L"std::u32string", L"std::pmr::basic_string", L"std::pmr::string",
            L"std::pmr::wstring", L"std::pmr::u8string", L"std::pmr::u16string",
            L"std::pmr::u32string", L"pmr::basic_string", L"pmr::string", L"pmr::wstring",
            L"pmr::u8string", L"pmr::u16string", L"pmr::u32string", L"std::ifstream",
            L"std::ofstream",
            // MFC, ATL
            L"CString", L"_bstr_t",
            // Java
            L"Locale",
            // formatting functions (not actually a CTOR) that should be skipped over
            L"wxString::Format"
        };

        // Debugging & system call functions that should never have
        // their string parameters translated. This can also include resource
        // loading functions that take a string ID.
        m_internal_functions = {
            // Java resource/key functions
            L"getBundle", L"getObject", L"handleGetObject", L"getString", L"getStringArray",
            L"containsKey",
            // attributes
            L"deprecated", L"nodiscard", L"_Pragma",
            // assert functions
            L"check_assertion", L"static_assert", L"assert", L"Assert", L"__android_log_assert",
            // wxWidgets functions and macros
            L"wxDEPRECATED_MSG", L"wxSTC_DEPRECATED_MACRO_VALUE", L"wxPG_DEPRECATED_MACRO_VALUE",
            L"GetExt", L"SetExt", L"XRCID", L"wxSystemOptions::GetOptionInt", L"WXTRACE",
            L"wxTrace", L"wxDATETIME_CHECK", L"wxASSERT", L"wxASSERT_MSG", L"wxASSERT_LEVEL_2",
            L"wxASSERT_LEVEL_2_MSG", L"wxOnAssert", L"wxCHECK", L"wxCHECK2", L"wxCHECK2_MSG",
            L"wxCHECK_MSG", L"wxCHECK_RET", L"wxCOMPILE_TIME_ASSERT", L"wxPROPERTY_FLAGS",
            L"wxPROPERTY", L"wxMISSING_IMPLEMENTATION", L"wxCOMPILE_TIME_ASSERT2", L"wxFAIL_MSG",
            L"wxFAILED_HRESULT_MSG", L"ExecCommand", L"CanExecCommand", L"IgnoreAppSubDir",
            L"put_designMode", L"SetExtension", L"wxSystemOptions::SetOption",
            L"wxFileName::CreateTempFileName", L"wxExecute", L"SetFailedWithLastError",
            L"wxIconHandler", L"wxBitmapHandler", L"OutputDumpLine", L"wxFileTypeInfo",
            L"TAG_HANDLER_BEGIN", L"FDEBUG", L"MDEBUG", L"wxVersionInfo", L"Platform::DebugPrintf",
            L"wxGetCommandOutput", L"SetKeyWords", L"AddDeveloper", L"AddDocWriter", L"AddArtist",
            L"AddTranslator", L"SetCopyright", L"MarkerSetBackground", L"SetProperty",
            L"SetAppName",
            L"GetTextExtent" /*Usually only used for measuring "phantoms" like 'Aq'*/,
            L"GetAttribute",
            // Qt
            L"Q_ASSERT", L"Q_ASSERT_X", L"qSetMessagePattern", L"qmlRegisterUncreatableMetaObject",
            L"addShaderFromSourceCode", L"QStandardPaths::findExecutable", L"QDateTime::fromString",
            L"QFileInfo", L"qCDebug", L"qDebug", L"Q_MOC_INCLUDE", L"Q_CLASSINFO",
            // Catch2
            L"TEST_CASE", L"BENCHMARK", L"TEMPLATE_TEST_CASE", L"SECTION", L"DYNAMIC_SECTION",
            L"REQUIRE", L"REQUIRE_THROWS_WITH", L"REQUIRE_THAT", L"CHECK", L"CATCH_ENFORCE",
            L"INFO", L"SUCCEED", L"SCENARIO", L"GIVEN", L"AND_GIVEN", L"WHEN", L"THEN",
            L"SCENARIO_METHOD", L"WARN", L"TEST_CASE_METHOD", L"Catch::Clara::Arg",
            L"Catch::TestCaseInfo", L"GENERATE", L"CATCH_INTERNAL_ERROR", L"CATCH_ERROR",
            L"CATCH_MAKE_MSG", L"INTERNAL_CATCH_DYNAMIC_SECTION", L"CATCH_RUNTIME_ERROR",
            L"CATCH_INTERNAL_ERROR", L"CATCH_REQUIRE_THROWS_WIT", L"CATCH_SUCCEED", L"CATCH_INFO",
            L"CATCH_UNSCOPED_INFO", L"CATCH_WARN", L"CATCH_SECTION",
            // CppUnit
            L"CPPUNIT_ASSERT", L"CPPUNIT_ASSERT_EQUAL", L"CPPUNIT_ASSERT_DOUBLES_EQUAL",
            // Google Test
            L"EXPECT_STREQ", L"EXPECT_STRNE", L"EXPECT_STRCASEEQ", L"EXPECT_STRCASENE",
            L"EXPECT_TRUE", L"EXPECT_THAT", L"EXPECT_FALSE", L"EXPECT_EQ", L"EXPECT_NE",
            L"EXPECT_LT", L"EXPECT_LE", L"EXPECT_GT", L"EXPECT_GE", L"ASSERT_STREQ",
            L"ASSERT_STRNE", L"ASSERT_STRCASEEQ", L"ASSERT_STRCASENE", L"ASSERT_TRUE",
            L"ASSERT_THAT", L"ASSERT_FALSE", L"ASSERT_EQ", L"ASSERT_NE", L"ASSERT_LT", L"ASSERT_LE",
            L"ASSERT_GT", L"ASSERT_GE",
            // JUnit asserts
            L"assertEquals", L"assertNotEquals", L"assertArrayEquals", L"assertTrue", L"assertNull",
            L"assertNotNull", L"assertThat", L"assertNotEquals", L"assertNotSame", L"assertSame",
            L"assertThrows", L"fail",
            // other testing frameworks
            L"do_test", L"run_check", L"GNC_TEST_ADD_FUNC", L"GNC_TEST_ADD", L"g_test_message",
            L"check_binary_op", L"check_binary_op_equal", L"MockProvider",
            // MAME
            L"TEST_INSTRUCTION", L"ASIO_CHECK", L"ASIO_ERROR", L"ASIO_HANDLER_CREATION",
            L"ASMJIT_DEPRECATED",
            // low-level printf functions
            L"wprintf", L"printf", L"sprintf", L"snprintf", L"fprintf", L"wxSnprintf",
            // KDE
            L"getDocumentProperty", L"setDocumentProperty",
            // GTK
            L"gtk_assert_dialog_append_text_column", L"gtk_assert_dialog_add_button_to",
            L"gtk_assert_dialog_add_button", L"g_object_set_property", L"gdk_atom_intern",
            L"g_object_class_override_property", L"g_object_get", L"g_assert_cmpstr",
            // TCL
            L"Tcl_PkgRequire", L"Tcl_PkgRequireEx",
            // debugging functions from various open-source projects
            L"check_assertion", L"print_debug", L"DPRINTF", L"print_warning", L"perror",
            L"LogDebug", L"DebugMsg",
            // system functions that don't process user messages
            L"fopen", L"getenv", L"setenv", L"system", L"run", L"exec", L"execute",
            // Unix calls
            L"popen", L"dlopen", L"dlsym", L"g_signal_connect", L"handle_system_error",
            // macOS calls
            L"CFBundleCopyResourceURL", L"sysctlbyname",
            // Windows/MFC calls
            L"OutputDebugString", L"OutputDebugStringA", L"OutputDebugStringW", L"QueryValue",
            L"ASSERT", L"_ASSERTE", L"TRACE", L"ATLTRACE", L"TRACE0", L"ATLTRACE2", L"ATLENSURE",
            L"ATLASSERT", L"VERIFY", L"LoadLibrary", L"LoadLibraryEx", L"LoadModule",
            L"GetModuleHandle", L"QueryDWORDValue", L"GetTempFileName", L"QueryMultiStringValue",
            L"SetMultiStringValue", L"GetTempDirectory", L"FormatGmt", L"GetProgIDVersion",
            L"GetProfileInt", L"WriteProfileInt", L"RegOpenKeyEx", L"QueryStringValue", L"lpVerb",
            L"Invoke", L"Invoke0", L"ShellExecute", L"GetProfileString", L"GetProcAddress",
            L"RegisterClipboardFormat", L"CreateIC", L"_makepath", L"_splitpath", L"VerQueryValue",
            L"CLSIDFromProgID", L"StgOpenStorage", L"InvokeN", L"CreateStream", L"DestroyElement",
            L"CreateStorage", L"OpenStream", L"CallMethod", L"PutProperty", L"GetProperty",
            L"HasProperty", L"SetRegistryKey", L"CreateDC",
            // .NET
            L"FindSystemTimeZoneById", L"CreateSpecificCulture", L"DebuggerDisplay", L"Debug.Fail",
            L"DeriveKey", L"Assert.Fail", L"Debug.Assert", L"Debug.Print", L"Debug.WriteLine",
            L"Debug.Write", L"Debug.WriteIf", L"Debug.WriteLineIf", L"Assert.Equal",
            // zlib
            L"Tracev", L"Trace", L"Tracevv",
            // Lua
            L"luaL_error", L"lua_pushstring", L"lua_setglobal",
            // more functions from various apps
            L"trace", L"ActionFormat", L"ErrorFormat", L"addPositionalArgument", L"DEBUG",
            L"setParameters"
        };

        m_log_functions = {
            // wxWidgets
            L"wxLogLastError", L"wxLogError", L"wxLogFatalError", L"wxLogGeneric", L"wxLogMessage",
            L"wxLogStatus", L"wxLogStatus", L"wxLogSysError", L"wxLogTrace", L"wxLogTrace",
            L"wxLogVerbose", L"wxLogWarning", L"wxLogDebug", L"wxLogApiError", L"LogTraceArray",
            L"DoLogRecord", L"DoLogText", L"DoLogTextAtLevel", L"LogRecord", L"DDELogError",
            L"LogTraceLargeArray",
            // Qt
            L"qDebug", L"qInfo", L"qWarning", L"qCritical", L"qFatal", L"qCDebug", L"qCInfo",
            L"qCWarning", L"qCCritical",
            // GLIB
            L"g_error", L"g_info", L"g_log", L"g_message", L"g_debug", L"g_message", L"g_warning",
            L"g_log_structured", L"g_critical",
            // SDL
            L"SDL_Log", L"SDL_LogCritical", L"SDL_LogDebug", L"SDL_LogError", L"SDL_LogInfo",
            L"SDL_LogMessage", L"SDL_LogMessageV", L"SDL_LogVerbose", L"SDL_LogWarn",
            // GnuCash
            L"PERR", L"PWARN", L"PINFO", L"ENTER", L"LEAVE",
            // actual console functions
            // (we will consider that most console apps are not localized, or if being used
            //  in a GUI then the message is meant for developers)
            L"printf", L"Console.WriteLine",
            // .NET
            L"LoggerMessage",
            // other programs
            L"log_message", L"outLog"
        };

        m_exceptions = {
            // std exceptions
            L"logic_error", L"std::logic_error", L"domain_error", L"std::domain_error",
            L"length_error", L"std::length_error", L"out_of_range", L"std::out_of_range",
            L"runtime_error", L"std::runtime_error", L"overflow_error", L"std::overflow_error",
            L"underflow_error", L"std::underflow_error", L"range_error", L"std::range_error",
            L"invalid_argument", L"std::invalid_argument", L"exception", L"std::exception",
            // MFC
            L"AfxThrowOleDispatchException"
        };

        // known strings to ignore
        m_known_internal_strings = { L"size-points", L"background-gdk", L"foreground-gdk",
                                     L"foreground-set", L"background-set", L"weight-set",
                                     L"style-set", L"underline-set", L"size-set", L"charset",
                                     L"xml", L"gdiplus", L"Direct2D", L"DirectX", L"localhost",
                                     L"32 bit", L"32-bit", L"64 bit", L"64-bit", L"NULL",
                                     // build types
                                     L"DEBUG", L"NDEBUG",
                                     // RTF font families
                                     L"fnil", L"fdecor", L"froman", L"fscript", L"fswiss",
                                     L"fmodern", L"ftech",
                                     // common UNIX names (Windows versions are handled by more
                                     // complex regex expressions elsewhere)
                                     L"UNIX", L"macOS", L"Apple Mac OS", L"Apple Mac OS X", L"OSX",
                                     L"Linux", L"FreeBSD", L"POSIX", L"NetBSD" };

        // keywords in the language that can appear in front of a string only
        m_keywords = { L"return", L"else", L"if", L"goto", L"new", L"delete", L"throw" };

        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(^debug.*)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(^stacktrace.*)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(([[:alnum:]_\-])*xpm)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(xpm([[:alnum:]_\-])*)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"((sql|db|database)(Table|Update|Query|Command|Upgrade)?[[:alnum:]_\-]*)",
                        std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(log)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(wxColourDialogNames)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(wxColourTable)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(QT_MESSAGE_PATTERN)"));
        // console objects
        add_variable_name_pattern_to_ignore(std::wregex(LR"((std::)?[w]?cout)"));
        }

    //--------------------------------------------------
    std::pair<bool, size_t> i18n_review::is_block_suppressed(std::wstring_view commentBlock)
        {
        const std::wstring_view SUPPRESS_BEGIN{ L"cuneiform-suppress-begin" };
        const std::wstring_view SUPPRESS_END{ L"cuneiform-suppress-end" };

        const size_t firstNonSpace = commentBlock.find_first_not_of(L" \t\n\r");
        if (firstNonSpace == std::wstring_view::npos)
            {
            return std::make_pair(false, std::wstring_view::npos);
            }
        commentBlock.remove_prefix(firstNonSpace);
        if (commentBlock.length() > SUPPRESS_BEGIN.length() &&
            commentBlock.compare(0, SUPPRESS_BEGIN.length(), SUPPRESS_BEGIN) == 0)
            {
            const size_t endOfBlock = commentBlock.find(SUPPRESS_END);
            if (endOfBlock == std::wstring_view::npos)
                {
                return std::make_pair(false, std::wstring_view::npos);
                }
            return std::make_pair(true, firstNonSpace + endOfBlock + SUPPRESS_END.length());
            }
        else
            {
            return std::make_pair(false, std::wstring_view::npos);
            }
        }

    //--------------------------------------------------
    void i18n_review::review_strings([[maybe_unused]] analyze_callback_reset resetCallback,
                                     [[maybe_unused]] analyze_callback callback)
        {
        process_strings();

        if (m_reviewStyles & check_l10n_contains_url)
            {
            std::wsmatch results;
            for (const auto& str : m_localizable_strings)
                {
                if (std::regex_search(str.m_string, results, m_url_email_regex))
                    {
                    m_localizable_strings_with_urls.push_back(str);
                    }
                }
            }

        if (m_reviewStyles & check_l10n_strings)
            {
            for (const auto& str : m_localizable_strings)
                {
                if (str.m_string.length() && is_untranslatable_string(str.m_string, false))
                    {
                    m_unsafe_localizable_strings.push_back(str);
                    }
                }
            }

        if (m_reviewStyles & check_malformed_strings)
            {
            const auto& classifyMalformedStrings = [this](const auto& strings)
            {
                for (const auto& str : strings)
                    {
                    if (std::regex_search(str.m_string, m_malformed_html_tag) ||
                        std::regex_search(str.m_string, m_malformed_html_tag_bad_amp))
                        {
                        m_malformed_strings.push_back(str);
                        }
                    }
            };

            classifyMalformedStrings(m_localizable_strings);
            classifyMalformedStrings(m_marked_as_non_localizable_strings);
            classifyMalformedStrings(m_internal_strings);
            classifyMalformedStrings(m_not_available_for_localization_strings);
            classifyMalformedStrings(m_unsafe_localizable_strings);
            classifyMalformedStrings(m_localizable_strings_with_urls);
            classifyMalformedStrings(m_localizable_strings_in_internal_call);
            }

        if (m_reviewStyles & check_unencoded_ext_ascii)
            {
            const auto& classifyUnencodedStrings = [this](const auto& strings)
            {
                for (const auto& str : strings)
                    {
                    for (const auto& ch : str.m_string)
                        {
                        if (ch >= 128)
                            {
                            m_unencoded_strings.push_back(str);
                            break;
                            }
                        }
                    }
            };

            classifyUnencodedStrings(m_localizable_strings);
            classifyUnencodedStrings(m_marked_as_non_localizable_strings);
            classifyUnencodedStrings(m_internal_strings);
            classifyUnencodedStrings(m_not_available_for_localization_strings);
            classifyUnencodedStrings(m_unsafe_localizable_strings);
            classifyUnencodedStrings(m_localizable_strings_with_urls);
            classifyUnencodedStrings(m_localizable_strings_in_internal_call);
            }

        if (m_reviewStyles & check_printf_single_number)
            {
            // only looking at integral values (i.e., no floating-point precision)
            std::wregex intPrintf{ LR"([%]([+]|[-] #0)?(l)?(d|i|o|u|zu|c|C|e|E|x|X|l|I|I32|I64))" };
            std::wregex floatPrintf{ LR"([%]([+]|[-] #0)?(l|L)?(f|F))" };
            const auto& classifyPrintfIntStrings = [&, this](const auto& strings)
            {
                for (const auto& str : strings)
                    {
                    if (std::regex_match(str.m_string, intPrintf) ||
                        std::regex_match(str.m_string, floatPrintf))
                        {
                        m_printf_single_numbers.push_back(str);
                        }
                    }
            };
            classifyPrintfIntStrings(m_internal_strings);
            classifyPrintfIntStrings(m_localizable_strings_in_internal_call);
            }

        // log any parsing errors
        run_diagnostics();
        }

    //--------------------------------------------------
    void i18n_review::classify_non_localizable_string(string_info str)
        {
        if (m_reviewStyles & check_not_available_for_l10n)
            {
            if (!should_exceptions_be_translatable() &&
                (m_exceptions.find(str.m_usage.m_value) != m_exceptions.cend() ||
                 m_exceptions.find(str.m_usage.m_variableType) != m_exceptions.cend()))
                {
                return;
                }
            if (m_log_functions.find(str.m_usage.m_value) != m_log_functions.cend())
                {
                return;
                }

            if (is_untranslatable_string(str.m_string, true))
                {
                m_internal_strings.emplace_back(str);
                }
            else
                {
                m_not_available_for_localization_strings.emplace_back(str);
                }
            }
        }

    //--------------------------------------------------
    std::wstring_view i18n_review::extract_base_function(const std::wstring_view str) const
        {
        if (str.empty() || !is_valid_name_char(str.back()))
            {
            return std::wstring_view{};
            }
        if (str.length() == 1)
            {
            return is_valid_name_char(str[0]) ? str : std::wstring_view{};
            }
        for (int64_t i = static_cast<int64_t>(str.length() - 1); i >= 0; --i)
            {
            if (!is_valid_name_char(str[static_cast<size_t>(i)]))
                {
                return str.substr(static_cast<size_t>(i) + 1, str.length());
                }
            }
        return str;
        }

    //--------------------------------------------------
    void i18n_review::load_deprecated_functions(const std::wstring_view fileText,
                                                const std::filesystem::path& fileName)
        {
        if (!static_cast<bool>(m_reviewStyles & check_deprecated_macros))
            {
            return;
            }

        for (size_t i = 0; i < fileText.length(); /* in loop*/)
            {
            for (const auto& func : m_deprecated_string_functions)
                {
                if (fileText.substr(i, func.first.length()).compare(func.first) == 0 &&
                    // ensure function is a whole-word match and has something after it
                    (i + func.first.length() < fileText.length() &&
                     !is_valid_name_char(fileText[i + func.first.length()])) &&
                    (i == 0 || !is_valid_name_char(fileText[i - 1])))
                    {
                    m_deprecated_macros.push_back(string_info(
                        std::wstring{ func.first.data(), func.first.length() },
                        string_info::usage_info(string_info::usage_info::usage_type::function,
                                                std::wstring(func.second), std::wstring{}),
                        fileName, get_line_and_column(i, fileText.data())));
                    i += func.first.length();
                    continue;
                    }
                }
            ++i;
            }
        }

    //--------------------------------------------------
    void i18n_review::load_id_assignments(const std::wstring_view fileText,
                                          const std::filesystem::path& fileName)
        {
        if (!(static_cast<bool>(m_reviewStyles & check_duplicate_value_assigned_to_ids) ||
              static_cast<bool>(m_reviewStyles & check_number_assigned_to_id)))
            {
            return;
            }
        std::vector<std::pair<size_t, std::wstring>> matches;
        std::match_results<decltype(fileText)::const_iterator> stPositions;
        auto currentTextBlock{ fileText };
        size_t currentBlockOffset{ 0 };
        while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(), stPositions,
                                 m_id_assignment_regex))
            {
            currentBlockOffset += stPositions.position();
            currentTextBlock = currentTextBlock.substr(stPositions.position());

            matches.emplace_back(
                currentBlockOffset,
                currentTextBlock.substr(0, stPositions.position() + stPositions.length()));

            currentBlockOffset += stPositions.length();

            currentTextBlock = currentTextBlock.substr(stPositions.length());
            }

        const std::wregex varNamePartsRE{ L"([a-zA-Z0-9_]*)(ID)([a-zA-Z0-9_]*)" };
        const std::wregex varNameIDPartsRE{ L"([a-zA-Z0-9_]*)(ID[A-Z]?[_]?)([a-zA-Z0-9_]*)" };
        // no std::from_chars for wchar_t :(
        const std::wregex numRE{ LR"(^[\-0-9']+$)" };
        if (!matches.empty())
            {
            std::vector<std::tuple<size_t, std::wstring, std::wstring>> idAssignments;
            idAssignments.reserve(matches.size());
            std::vector<std::wstring> subMatches;
            std::vector<std::wstring> idNameParts;
            // ID and variable name assigned to
            std::map<std::wstring, std::wstring> assignedIds;
            for (const auto& match : matches)
                {
                subMatches.clear();
                idNameParts.clear();
                auto varAssignment{ match.second };
                // get the var name and ID
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(varAssignment)>::const_iterator>(
                              varAssignment.cbegin(), varAssignment.cend(), m_id_assignment_regex,
                              { 3, 4, 5 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(varAssignment)>::const_iterator>{},
                          std::back_inserter(subMatches));
                // ignore function calls or constructed objects assigning an ID
                if (subMatches[2] == L"(" || subMatches[2] == L"{")
                    {
                    continue;
                    }
                // clean up ID (e.g., 1'000 -> 1000)
                string_util::remove_all(subMatches[1], L'\'');
                string_util::trim(subMatches[1]);
                string_util::remove_all(subMatches[1], L' ');
                // break the ID into parts and see what's around "ID";
                // we don't want "ID" if it is part of a word like "WIDTH"
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(subMatches[0])>::const_iterator>(
                              subMatches[0].cbegin(), subMatches[0].cend(), varNamePartsRE,
                              { 1, 2, 3, 4, 5 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(subMatches[0])>::const_iterator>{},
                          std::back_inserter(idNameParts));
                // MFC IDs
#if __cplusplus >= 202002L
                if ((idNameParts[0].empty() ||
                     (idNameParts[0].length() > 0 &&
                      !static_cast<bool>(std::iswupper(idNameParts[0].back())))) &&
                    (idNameParts[2].starts_with(L"R_") || idNameParts[2].starts_with(L"D_") ||
                     idNameParts[2].starts_with(L"C_") || idNameParts[2].starts_with(L"I_") ||
                     idNameParts[2].starts_with(L"B_") || idNameParts[2].starts_with(L"S_") ||
                     idNameParts[2].starts_with(L"M_") || idNameParts[2].starts_with(L"P_")))
                    {
                    idAssignments.push_back({ match.first, subMatches[0], subMatches[1] });
                    continue;
                    }
#endif
                if ((idNameParts[0].length() > 0 &&
                     static_cast<bool>(std::iswupper(idNameParts[0].back()))) ||
                    (idNameParts[2].length() > 0 &&
                     static_cast<bool>(std::iswupper(idNameParts[2].front()))))
                    {
                    continue;
                    }

                idAssignments.push_back({ match.first, subMatches[0], subMatches[1] });
                }
            for (const auto& [position, string1, string2] : idAssignments)
                {
                idNameParts.clear();
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(string1)>::const_iterator>(
                              string1.cbegin(), string1.cend(), varNameIDPartsRE, { 1, 2, 3 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(string1)>::const_iterator>(),
                          std::back_inserter(idNameParts));
                const auto idVal = [&string2]()
                {
                    try
                        {
                        if (string2.starts_with(L"0x"))
                            {
                            return std::optional<int32_t>(std::stol(string2, nullptr, 16));
                            }
                        return std::optional<int32_t>(std::stol(string2));
                        }
                    catch (...)
                        {
                        return std::optional<int32_t>{ std::nullopt };
                        }
                }();
                const int32_t idRangeStart{ 1 };
                const int32_t menuIdRangeEnd{ 0x6FFF };
                const int32_t stringIdRangeEnd{ 0x7FFF };
                const int32_t dialogIdRangeStart{ 8 };
                const int32_t dialogIdRangeEnd{ 0xDFFF };
                if (static_cast<bool>(m_reviewStyles & check_number_assigned_to_id) && idVal &&
                    !(idVal.value() >= idRangeStart && idVal.value() <= menuIdRangeEnd) &&
                    (idNameParts[1] == L"IDR_" || idNameParts[1] == L"IDD_" ||
                     idNameParts[1] == L"IDM_" || idNameParts[1] == L"IDC_" ||
                     idNameParts[1] == L"IDI_" || idNameParts[1] == L"IDB_"))
                    {
                    m_ids_assigned_number.push_back(string_info(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s; value should be between 1 and "
                                           "0x6FFF if this is an MFC project."),
                                         string2, string1)
                            .wc_str(),
#else
                        string2 + _DT(L" assigned to ") + string1 +
                            _DT(L"; value should be between 1 and 0x6FFF if "
                                L"this is an MFC project."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos)));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_number_assigned_to_id) && idVal &&
                         !(idVal.value() >= idRangeStart && idVal.value() <= stringIdRangeEnd) &&
                         (idNameParts[1] == L"IDS_" || idNameParts[1] == L"IDP_"))
                    {
                    m_ids_assigned_number.emplace_back(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s; value should be between 1 and "
                                           "0x7FFF if this is an MFC project."),
                                         string2, string1)
                            .wc_str(),
#else
                        string2 + _DT(L" assigned to ") + string1 +
                            _DT(L"; value should be between 1 and 0x7FFF if "
                                "this is an MFC project."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_number_assigned_to_id) && idVal &&
                         !(idVal.value() >= dialogIdRangeStart &&
                           idVal.value() <= dialogIdRangeEnd) &&
                         idNameParts[1] == L"IDC_")
                    {
                    m_ids_assigned_number.emplace_back(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s; value should be between 8 and "
                                           "0xDFFF if this is an MFC project."),
                                         string2, string1)
                            .wc_str(),
#else
                        string2 + _DT(L" assigned to ") + string1 +
                            _DT(L"; value should be between 8 and 0xDFFF if "
                                "this is an MFC project."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_number_assigned_to_id) &&
                         idNameParts[1].length() <= 3 && // ignore MFC IDs (handled above)
                         std::regex_match(string2, numRE) &&
                         // -1 or 0 are usually generic IDs for the framework or
                         // temporary init values
                         string2 != L"-1" && string2 != L"0")
                    {
                    m_ids_assigned_number.emplace_back(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s"), string2, string1).wc_str(),
#else
                        string2 + _DT(L" assigned to ") + string1,
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos));
                    }

                const auto [pos, inserted] = assignedIds.insert(std::make_pair(string2, string1));

                if (static_cast<bool>(m_reviewStyles & check_duplicate_value_assigned_to_ids) &&
                    !inserted && string2.length() > 0 &&
                    // ignore if same ID is assigned to variables with the same name
                    string1 != pos->second && string2 != L"wxID_ANY" && string2 != L"wxID_NONE" &&
                    string2 != L"-1" && string2 != L"0")
                    {
                    m_duplicates_value_assigned_to_ids.push_back(string_info(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s has been assigned to multiple ID variables."),
                                         string2)
                            .wc_str(),
#else
                        string2 + _DT(L" has been assigned to multiple ID variables."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos)));
                    }
                }
            }
        }

    //--------------------------------------------------
    void i18n_review::process_quote(wchar_t* currentTextPos, const wchar_t* quoteEnd,
                                    const wchar_t* functionVarNamePos,
                                    const std::wstring& variableName,
                                    const std::wstring& functionName,
                                    const std::wstring& variableType,
                                    const std::wstring& deprecatedMacroEncountered,
                                    const size_t parameterPosition)
        {
        if (deprecatedMacroEncountered.length() > 0 &&
            static_cast<bool>(m_reviewStyles & check_deprecated_macros))
            {
            const auto foundMessage = m_deprecated_string_macros.find(deprecatedMacroEncountered);
            m_deprecated_macros.emplace_back(
                deprecatedMacroEncountered,
                string_info::usage_info(string_info::usage_info::usage_type::function,
                                        (foundMessage != m_deprecated_string_macros.cend()) ?
                                            std::wstring{ foundMessage->second } :
                                            std::wstring{},
                                        std::wstring{}),
                m_file_name, get_line_and_column(currentTextPos - m_file_start));
            }

        if (variableName.length() > 0)
            {
            process_variable(variableType, variableName,
                             std::wstring(currentTextPos, quoteEnd - currentTextPos),
                             (currentTextPos - m_file_start));
            }
        else if (functionName.length() > 0)
            {
            if (is_diagnostic_function(functionName))
                {
                m_internal_strings.emplace_back(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start));
                }
            else if (is_i18n_function(functionName))
                {
                // i18n functions that have multiple params, some of which
                // can be context strings that should not be translatable
                if ( // Qt
                    (functionName == _DT(L"translate") && parameterPosition == 0) ||
                    (functionName == L"QApplication::translate" && parameterPosition == 0) ||
                    (functionName == L"QApplication::tr" && parameterPosition == 0) ||
                    (functionName == L"QApplication::trUtf8" && parameterPosition == 0) ||
                    (functionName == L"QCoreApplication::translate" && parameterPosition == 0) ||
                    (functionName == L"QCoreApplication::tr" && parameterPosition == 0) ||
                    (functionName == L"QCoreApplication::trUtf8" && parameterPosition == 0) ||
                    (functionName == L"tr" && parameterPosition == 1) ||
                    (functionName == L"trUtf8" && parameterPosition == 1) ||
                    (functionName == L"QT_TRANSLATE_NOOP" && parameterPosition == 0) ||
                    // wxWidgets
                    (functionName == L"wxTRANSLATE_IN_CONTEXT" && parameterPosition == 0) ||
                    (functionName == L"wxGETTEXT_IN_CONTEXT_PLURAL" && parameterPosition == 0) ||
                    (functionName == L"wxGETTEXT_IN_CONTEXT" && parameterPosition == 0) ||
                    (functionName == L"wxGetTranslation" &&
                     (parameterPosition == 1 || parameterPosition == 3 || parameterPosition == 4)))
                    {
                    m_internal_strings.emplace_back(
                        std::wstring(currentTextPos, quoteEnd - currentTextPos),
                        string_info::usage_info(string_info::usage_info::usage_type::function,
                                                functionName, std::wstring{}),
                        m_file_name, get_line_and_column(currentTextPos - m_file_start));
                    }
                else
                    {
                    m_localizable_strings.emplace_back(
                        std::wstring(currentTextPos, quoteEnd - currentTextPos),
                        string_info::usage_info(string_info::usage_info::usage_type::function,
                                                functionName, std::wstring{}),
                        m_file_name, get_line_and_column(currentTextPos - m_file_start));

                    assert(functionVarNamePos);
                    if (functionVarNamePos != nullptr &&
                        static_cast<bool>(m_reviewStyles & check_suspect_l10n_string_usage))
                        {
                        std::wstring functionNameOuter;
                        std::wstring variableNameOuter;
                        std::wstring variableTypeOuter;
                        std::wstring deprecatedMacroOuterEncountered;
                        size_t parameterPositionOuter{ 0 };
                        read_var_or_function_name(
                            functionVarNamePos, m_file_start, functionNameOuter, variableNameOuter,
                            variableTypeOuter, deprecatedMacroOuterEncountered,
                            parameterPositionOuter);
                        if (deprecatedMacroOuterEncountered.length() > 0 &&
                            static_cast<bool>(m_reviewStyles & check_deprecated_macros))
                            {
                            m_deprecated_macros.emplace_back(
                                deprecatedMacroOuterEncountered,
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::function, std::wstring{},
                                    std::wstring{}),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start));
                            }
                        // internal functions
                        if (is_diagnostic_function(functionNameOuter) ||
                            // CTORs whose arguments should not be translated
                            m_variable_types_to_ignore.find(functionNameOuter) !=
                                m_variable_types_to_ignore.cend())
                            {
                            m_localizable_strings_in_internal_call.emplace_back(
                                std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::function,
                                    functionNameOuter, std::wstring{}),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start));
                            }
                        // untranslatable variable types
                        else if (m_variable_types_to_ignore.find(variableTypeOuter) !=
                                 m_variable_types_to_ignore.cend())
                            {
                            m_localizable_strings_in_internal_call.emplace_back(
                                std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::variable,
                                    variableNameOuter, variableTypeOuter),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start));
                            }
                        // untranslatable variable names (e.g., debugMsg)
                        else if (variableNameOuter.length() > 0)
                            {
                            try
                                {
                                for (const auto& reg : get_ignored_variable_patterns())
                                    {
                                    if (std::regex_match(variableNameOuter, reg))
                                        {
                                        m_localizable_strings_in_internal_call.emplace_back(
                                            std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                            string_info::usage_info(
                                                string_info::usage_info::usage_type::variable,
                                                variableNameOuter, variableTypeOuter),
                                            m_file_name,
                                            get_line_and_column(currentTextPos - m_file_start));
                                        break;
                                        }
                                    }
                                }
                            catch (const std::exception& exp)
                                {
                                log_message(variableNameOuter,
                                            i18n_string_util::lazy_string_to_wstring(exp.what()),
                                            (currentTextPos - m_file_start));
                                }
                            }
                        }
                    }
                }
            else if (is_non_i18n_function(functionName))
                {
                m_marked_as_non_localizable_strings.emplace_back(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start));
                }
            else if (m_variable_types_to_ignore.find(functionName) !=
                     m_variable_types_to_ignore.cend())
                {
                m_internal_strings.emplace_back(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start));
                }
            else if (is_keyword(functionName))
                {
                classify_non_localizable_string(
                    string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                        std::wstring{}, std::wstring{}),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start)));
                }
            else
                {
                classify_non_localizable_string(string_info(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start)));
                }
            }
        else
            {
            classify_non_localizable_string(
                string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                            string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                    std::wstring{}, std::wstring{}),
                            m_file_name, get_line_and_column(currentTextPos - m_file_start)));
            }
        clear_section(currentTextPos, std::next(quoteEnd));
        }

    //--------------------------------------------------
    void i18n_review::process_variable(const std::wstring& variableType,
                                       const std::wstring& variableName, const std::wstring& value,
                                       const size_t quotePosition)
        {
#ifndef NDEBUG
        if (variableType.length() > 0 &&
            get_ignored_variable_types().find(variableType) ==
                get_ignored_variable_types().cend() &&
            m_ctors_to_ignore.find(variableType) == m_ctors_to_ignore.cend() &&
            (variableType.length() < 5 ||
             std::wstring_view(variableType.c_str(), 5).compare(std::wstring_view{ L"std::", 5 }) !=
                 0) &&
            (variableType.length() < 2 ||
             std::wstring_view(variableType.c_str(), 2).compare(std::wstring_view{ L"wx", 2 }) !=
                 0) &&
            (variableType.length() < 3 ||
             std::wstring_view(variableType.c_str(), 3).compare(std::wstring_view{ L"_wx", 3 }) !=
                 0) &&
            (variableType.length() < 2 ||
             std::wstring_view(variableType.c_str(), 2).compare(std::wstring_view{ L"My", 2 }) !=
                 0) &&
            variableType != L"auto" && variableType != L"Bar" && variableType != L"ifstream" &&
            variableType != L"ofstream" && variableType != L"const" && variableType != L"char" &&
            variableType != L"int" && variableType != L"UINT" && variableType != L"size_t" &&
            variableType != L"float" && variableType != L"double" &&
            variableType != L"static_cast" && variableType != L"StyleInfo" &&
            variableType != L"Utf8CharBuffer" && variableType != L"rgbRecord" &&
            variableType != L"LPCTSTR" && variableType != L"CDialog" &&
            variableType != L"LanguageInfo" && variableType != L"MessageParameters")
            {
            log_message(variableType, L"New variable type detected.", quotePosition);
            }
#endif
        if (get_ignored_variable_types().find(variableType) != get_ignored_variable_types().cend())
            {
            m_internal_strings.emplace_back(
                value,
                string_info::usage_info(string_info::usage_info::usage_type::variable, variableName,
                                        variableType),
                m_file_name, get_line_and_column(quotePosition));
            return;
            }
        if (!get_ignored_variable_patterns().empty())
            {
            try
                {
                bool matchedInternalVar(false);
                for (const auto& reg : get_ignored_variable_patterns())
                    {
                    if (std::regex_match(variableName, reg))
                        {
                        m_internal_strings.emplace_back(
                            value,
                            string_info::usage_info(string_info::usage_info::usage_type::variable,
                                                    variableName, variableType),
                            m_file_name, get_line_and_column(quotePosition));
                        matchedInternalVar = true;
                        break;
                        }
                    }
                // didn't match any known internal variable name provided by user?
                if (!matchedInternalVar)
                    {
                    classify_non_localizable_string(string_info(
                        value,
                        string_info::usage_info(string_info::usage_info::usage_type::variable,
                                                variableName, variableType),
                        m_file_name, get_line_and_column(quotePosition)));
                    }
                }
            catch (const std::exception& exp)
                {
                log_message(variableName, i18n_string_util::lazy_string_to_wstring(exp.what()),
                            quotePosition);
                classify_non_localizable_string(string_info(
                    value,
                    string_info::usage_info(string_info::usage_info::usage_type::variable,
                                            variableName, variableType),
                    m_file_name, get_line_and_column(quotePosition)));
                }
            }
        else
            {
            classify_non_localizable_string(
                string_info(value,
                            string_info::usage_info(string_info::usage_info::usage_type::variable,
                                                    variableName, variableType),
                            m_file_name, get_line_and_column(quotePosition)));
            }
        }

    //--------------------------------------------------
    void i18n_review::reserve(const size_t fileCount)
        {
        try
            {
            m_error_log.reserve(std::min<size_t>(fileCount, 10));
            m_localizable_strings.reserve(std::min<size_t>(fileCount, 10));
            m_not_available_for_localization_strings.reserve(std::min<size_t>(fileCount, 10));
            m_marked_as_non_localizable_strings.reserve(std::min<size_t>(fileCount, 10));
            m_internal_strings.reserve(std::min<size_t>(fileCount, 10));
            m_unsafe_localizable_strings.reserve(std::min<size_t>(fileCount, 10));
            m_deprecated_macros.reserve(std::min<size_t>(fileCount, 10));
            }
        catch (const std::bad_alloc&)
            {
            }
        }

    //--------------------------------------------------
    void i18n_review::clear_results() noexcept
        {
        m_localizable_strings.clear();
        m_not_available_for_localization_strings.clear();
        m_marked_as_non_localizable_strings.clear();
        m_internal_strings.clear();
        m_unsafe_localizable_strings.clear();
        m_deprecated_macros.clear();
        }

    //--------------------------------------------------
    bool i18n_review::is_diagnostic_function(const std::wstring& functionName) const
        {
        try
            {
            return (std::regex_match(functionName, m_diagnostic_function_regex) ||
                    (m_internal_functions.find(functionName) != m_internal_functions.cend()) ||
                    (m_internal_functions.find(extract_base_function(functionName)) !=
                     m_internal_functions.cend()) ||
                    (!can_log_messages_be_translatable() &&
                     m_log_functions.find(functionName) != m_log_functions.cend()));
            }
        catch (const std::exception& exp)
            {
            log_message(functionName, i18n_string_util::lazy_string_to_wstring(exp.what()),
                        std::wstring::npos);
            return true;
            }
        }

    //--------------------------------------------------
    bool i18n_review::is_untranslatable_string(const std::wstring& strToReview,
                                               const bool limitWordCount) const
        {
        // if no spaces but lengthy, then this is probably some sort of GUID
        if (strToReview.find(L' ') == std::wstring::npos &&
            strToReview.length() > 100)
            {
            return true;
            }

        std::wstring str{ strToReview };
        static const std::wregex oneWordRE{ LR"((\b[a-zA-Z&'\-]+([\.\-\/:]*[\w'\-]*)*))" };
        static const std::wregex loremIpsum(L"Lorem ipsum.*");

        i18n_string_util::replace_escaped_control_chars(str);
        string_util::trim(str);
        // see if a function signature before stripping printf commands and whatnot
        if ((std::regex_match(str, m_function_signature_regex) ||
             std::regex_match(str, m_open_function_signature_regex)) &&
            // but allow something like "Item(s)"
            !std::regex_match(str, m_plural_regex))
            {
            return true;
            }

        i18n_string_util::remove_hex_color_values(str);
        i18n_string_util::remove_printf_commands(str);
        i18n_string_util::remove_escaped_unicode_values(str);
        string_util::trim(str);
        // strip control characters (these wreak havoc with the regex parser)
        for (auto& chr : str)
            {
            if (chr == L'\n' || chr == L'\t' || chr == L'\r')
                {
                chr = L' ';
                }
            }
        string_util::trim(str);

        try
            {
            // Handle HTML syntax that is hard coded in the source file.
            // Strip it down and then see if what's left contains translatable content.
            // Note that we skip any punctuation (not word characters, excluding '<')
            // in front of the initial '<' (sometimes there are braces and brackets
            // in front of the HTML tags).
            str = std::regex_replace(str, std::wregex(LR"(<br[[:space:]]*\/>)"), L"\n");
            string_util::trim(str);
            if (std::regex_match(str, m_xml_element_regex) || std::regex_match(str, m_html_regex) ||
                std::regex_match(str, m_html_element_with_content_regex) ||
                std::regex_match(str, m_html_tag_regex) ||
                std::regex_match(str, m_html_tag_unicode_regex))
                {
                str = std::regex_replace(str, std::wregex(LR"(<script[\d\D]*?>[\d\D]*?</script>)"),
                                         L"");
                str = std::regex_replace(str, std::wregex(LR"(<style[\d\D]*?>[\d\D]*?</style>)"),
                                         L"");
                str = std::regex_replace(
                    str, std::wregex(L"<[?]?[A-Za-z0-9+_/\\-\\.'\"=;:!%[:space:]\\\\,()]+[?]?>"),
                    L"");
                str = std::regex_replace(str, m_xml_element_regex, L"");
                // strip things like &ldquo;
                str = std::regex_replace(str, std::wregex(L"&[[:alpha:]]{2,5};"), L"");
                str = std::regex_replace(str, std::wregex(L"&#[[:digit:]]{2,4};"), L"");
                }

            if (limitWordCount)
                {
                // see if it has enough words
                const auto matchCount{ std::distance(
                    std::wsregex_iterator(str.cbegin(), str.cend(), oneWordRE),
                    std::wsregex_iterator()) };
                if (static_cast<size_t>(matchCount) <
                    get_min_words_for_classifying_unavailable_string())
                    {
                    return true;
                    }
                }

            // Nothing but punctuation? If that's OK to allow, then let it through.
            if (is_allowing_translating_punctuation_only_strings() &&
                std::regex_match(str, std::wregex(L"[[:punct:]]+")))
                {
                return false;
                }

            // "N/A", "O&n", and "O&K" are OK to translate, but it won't meet the criterion of at
            // least two consecutive letters, so check for that first.
            if (str.length() == 3 &&
                ((string_util::is_either(str[0], L'N', L'n') && str[1] == L'/' &&
                  string_util::is_either(str[2], L'A', L'a')) ||
                 (string_util::is_either(str[0], L'O', L'o') && str[1] == L'&' &&
                  (string_util::is_either(str[2], L'N', L'n') ||
                   string_util::is_either(str[2], L'K', L'k')))))
                {
                return false;
                }
            constexpr size_t maxWordSize{ 20 };
            if (str.length() <= 1 ||
                // not at least two letters together
                !std::regex_search(str, m_2letter_regex) ||
                // single word (no spaces or word separators) and more than 20 characters--
                // doesn't seem like a real word meant for translation
                (str.length() > maxWordSize &&
                 str.find_first_of(L" \n\t\r/-") == std::wstring::npos &&
                 str.find(L"\\n") == std::wstring::npos && str.find(L"\\r") == std::wstring::npos &&
                 str.find(L"\\t") == std::wstring::npos) ||
                m_known_internal_strings.find(str.c_str()) != m_known_internal_strings.end() ||
                // a string like "_tcscoll" be odd to be in string, but just in case it
                // should not be localized
                m_deprecated_string_functions.find(str.c_str()) !=
                    m_deprecated_string_functions.end() ||
                m_deprecated_string_macros.find(str.c_str()) != m_deprecated_string_macros.end())
                {
                return true;
                }
            // RTF text
            if (str.compare(0, 3, LR"({\\)") == 0)
                {
                return true;
                }
            // social media hashtag (or formatting code of some sort)
            if (std::regex_match(str, m_hashtag_regex))
                {
                return true;
                }
            if (std::regex_match(str, m_key_shortcut_regex))
                {
                return true;
                }
            constexpr size_t minMessageLength{ 200 };
            // if we know it had at least one word (and spaces) at this point,
            // then it being more than 200 characters means that it probably is
            // a real user-message (not an internal string)
            if (str.length() > minMessageLength && !std::regex_match(str, loremIpsum) &&
                !std::regex_match(str, m_sql_code))
                {
                return false;
                }

            if (m_untranslatable_exceptions.find(str) != m_untranslatable_exceptions.cend())
                {
                return false;
                }

            // strings that may look like they should not be translatable, but are actually OK
            for (const auto& reg : m_translatable_regexes)
                {
                if (std::regex_match(str, reg))
                    {
#ifndef NDEBUG
                    if (str.length() > m_longest_internal_string.first.length())
                        {
                        m_longest_internal_string.first = str;
                        m_longest_internal_string.second = reg;
                        }
#endif
                    return false;
                    }
                }

            for (const auto& reg : m_untranslatable_regexes)
                {
                if (std::regex_match(str, reg))
                    {
#ifndef NDEBUG
                    if (str.length() > m_longest_internal_string.first.length())
                        {
                        m_longest_internal_string.first = str;
                        m_longest_internal_string.second = reg;
                        }
#endif
                    return true;
                    }
                }
            return (is_font_name(str.c_str()) || is_file_extension(str.c_str()) ||
                    i18n_string_util::is_file_address(str));
            }
        catch (const std::exception& exp)
            {
            log_message(str, i18n_string_util::lazy_string_to_wstring(exp.what()),
                        std::wstring::npos);
            return false;
            }
        }

    //--------------------------------------------------
    std::wstring i18n_review::collapse_multipart_string(std::wstring& str)
        {
        // for strings that span multiple lines, remove the start/end quotes and newlines
        // between them, combining this into one string
        const std::wregex multilineRegex(LR"(([^\\])("[\s]+"))");
        str = std::regex_replace(str, multilineRegex, L"$1");
        // replace any doubled-up quotes with single
        // (C# does this for raw strings)
        if (m_collapse_double_quotes)
            {
            string_util::replace_all<std::wstring>(str, LR"("")", LR"(")");
            }
        return str;
        }

    //--------------------------------------------------
    void i18n_review::process_strings()
        {
        const auto processStrings = [this](auto& strings)
        {
            std::for_each(strings.begin(), strings.end(),
                          [this](auto& val)
                          { val.m_string = i18n_review::collapse_multipart_string(val.m_string); });
        };
        processStrings(m_localizable_strings);
        processStrings(m_localizable_strings_in_internal_call);
        processStrings(m_not_available_for_localization_strings);
        processStrings(m_marked_as_non_localizable_strings);
        processStrings(m_internal_strings);
        processStrings(m_unsafe_localizable_strings);
        }

    //--------------------------------------------------
    void i18n_review::run_diagnostics()
        {
        for (const auto& str : m_localizable_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_not_available_for_localization_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_marked_as_non_localizable_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_internal_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_unsafe_localizable_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        }

    //--------------------------------------------------
    const wchar_t* i18n_review::read_var_or_function_name(
        const wchar_t* startPos, const wchar_t* const startSentinel, std::wstring& functionName,
        std::wstring& variableName, std::wstring& variableType,
        std::wstring& deprecatedMacroEncountered, size_t& parameterPosition)
        {
        functionName.clear();
        variableName.clear();
        variableType.clear();
        parameterPosition = 0;
        deprecatedMacroEncountered.clear();
        int32_t closeParenCount{ 0 };
        int32_t closeBraseCount{ 0 };
        bool quoteWrappedInCTOR{ false };
        const wchar_t* functionOrVarNamePos = startPos;

        /// @todo experimental!!! Reads the variable type from a variable constructed from a string.
        const auto readVarType = [&]()
        {
            const auto loadVarType = [&]()
            {
                variableType.clear();
                if (functionOrVarNamePos == startSentinel)
                    {
                    return;
                    }
                std::advance(functionOrVarNamePos, -1);
                while (functionOrVarNamePos > startSentinel &&
                       static_cast<bool>(std::iswspace(*functionOrVarNamePos)))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                const auto* typeEnd = std::next(functionOrVarNamePos);
                // if a template, then step over (going backwards) the template arguments
                // to get to the root type
                if (std::prev(typeEnd) > startSentinel && *std::prev(typeEnd) == L'>')
                    {
                    // if a pointer accessor (->) then bail as it won't be a variable assignment
                    if (std::prev(typeEnd, 2) > startSentinel && *std::prev(typeEnd, 2) == L'-')
                        {
                        return;
                        }
                    assert(functionOrVarNamePos >= startSentinel);
                    const auto openingAngle = std::wstring_view{ startSentinel }.find_last_of(
                        L'<', functionOrVarNamePos - startSentinel);
                    if (openingAngle == std::wstring::npos)
                        {
                        log_message(L"Template parse error",
                                    L"Unable to find opening < for template variable.",
                                    functionOrVarNamePos - startSentinel);
                        return;
                        }
                    functionOrVarNamePos =
                        std::next(startSentinel, static_cast<ptrdiff_t>(openingAngle));
                    }
                while (
                    functionOrVarNamePos > startSentinel &&
                    (is_valid_name_char_ex(*functionOrVarNamePos) || *functionOrVarNamePos == L'&'))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                if (!is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, 1);
                    }
                variableType.assign(functionOrVarNamePos, typeEnd - functionOrVarNamePos);
                // make sure the variable type is a word, not something like "<<"
                if (variableType.length() > 0 &&
                    !static_cast<bool>(std::iswalpha(variableType.front())))
                    {
                    variableType.clear();
                    }
                remove_decorations(variableType);
            };

            loadVarType();
            if (is_variable_type_decorator(variableType))
                {
                loadVarType();
                }

            // ignore case labels, else commands, etc.
            if (is_keyword(variableType) ||
                (variableType.length() > 0 && variableType.back() == L':'))
                {
                variableType.clear();
                }
        };

        while (startPos > startSentinel)
            {
            if (*startPos == L')')
                {
                ++closeParenCount;
                std::advance(startPos, -1);
                }
            else if (*startPos == L'}')
                {
                ++closeBraseCount;
                std::advance(startPos, -1);
                }
            else if (*startPos == L'(' || *startPos == L'{')
                {
                const auto currentOpeningChar{ *startPos };
                std::advance(startPos, -1);
                // if just closing the terminating parenthesis for a function
                // call in the list of parameters, then skip it and keep going
                // to find the outer function call that this string really belongs to.
                if (currentOpeningChar == L'(')
                    {
                    --closeParenCount;
                    }
                else if (currentOpeningChar == L'{')
                    {
                    --closeBraseCount;
                    }
                if (closeParenCount >= 0 && closeBraseCount >= 0)
                    {
                    continue;
                    }
                // skip whitespace between open parenthesis and function name
                while (startPos > startSentinel && static_cast<bool>(std::iswspace(*startPos)))
                    {
                    std::advance(startPos, -1);
                    }
                functionOrVarNamePos = startPos;
                while (functionOrVarNamePos > startSentinel &&
                       is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                // If we are on the start of the text, then see if we need to
                // include that character too. We may have short circuited because
                // we reached the start of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, 1);
                    }
                functionName.assign(functionOrVarNamePos,
                                    std::next(startPos) - functionOrVarNamePos);
                const bool hasExtraneousParens{ functionName.empty() };
                remove_decorations(functionName);
                // If wrapped in a string CTOR (e.g., std::wstring), then skip it
                // and keep going backwards.
                // Or, if no function name probably means extraneous parentheses, so keep going.
                if (hasExtraneousParens ||
                    m_ctors_to_ignore.find(functionName) != m_ctors_to_ignore.cend())
                    {
                    startPos = std::min(startPos, functionOrVarNamePos);
                    // reset, the current open parenthesis isn't relevant
                    if (currentOpeningChar == L'(')
                        {
                        closeParenCount = 0;
                        }
                    else if (currentOpeningChar == L'{')
                        {
                        closeBraseCount = 0;
                        }
                    if (m_deprecated_string_macros.find(functionName) !=
                        m_deprecated_string_macros.cend())
                        {
                        deprecatedMacroEncountered = functionName;
                        }
                    functionName.clear();
                    // now we should be looking for a + operator, comma, or ( or { proceeding this
                    // (unless we are already on it because we stepped back too far
                    //      due to the string being inside of an empty parenthesis)
                    if (*startPos != L',' && *startPos != L'+' && *startPos != L'&' &&
                        *startPos != L'=')
                        {
                        quoteWrappedInCTOR = true;
                        }
                    if (!hasExtraneousParens)
                        {
                        std::advance(startPos, -1);
                        }
                    continue;
                    }
                // construction of a variable type that takes
                // non-localizable strings, just skip it entirely
                if (m_variable_types_to_ignore.find(functionName) !=
                    m_variable_types_to_ignore.cend())
                    {
                    break;
                    }

                if (functionName.length() > 0)
                    {
                    // see if function is actually a CTOR
                    if (variableName.empty() &&
                        m_localization_functions.find(functionName) ==
                            m_localization_functions.cend() &&
                        m_non_localizable_functions.find(functionName) ==
                            m_non_localizable_functions.cend() &&
                        m_internal_functions.find(functionName) == m_internal_functions.cend() &&
                        m_log_functions.find(functionName) == m_log_functions.cend() &&
                        functionOrVarNamePos >= startSentinel && !is_keyword(functionName))
                        {
                        readVarType();

                        if (variableType.length() > 0)
                            {
                            variableName = functionName;
                            functionName.clear();
                            }
                        }
                    break;
                    }
                }
            // deal with variable assignments here
            // (note that comparisons (>=, <=, ==, !=) are handled as though this string
            //      is a parameter to a function.)
            else if (*startPos == L'=' && *std::next(startPos) != L'=' &&
                     startPos > startSentinel && *std::prev(startPos) != L'=' &&
                     *std::prev(startPos) != L'!' && *std::prev(startPos) != L'>' &&
                     *std::prev(startPos) != L'<')
                {
                std::advance(startPos, -1);
                // skip spaces (and "+=" tokens)
                while (startPos > startSentinel &&
                       (static_cast<bool>(std::iswspace(*startPos)) || *startPos == L'+'))
                    {
                    std::advance(startPos, -1);
                    }
                // skip array info
                if (startPos > startSentinel && *startPos == L']')
                    {
                    while (startPos > startSentinel && *startPos != L'[')
                        {
                        std::advance(startPos, -1);
                        }
                    std::advance(startPos, -1);
                    while (startPos > startSentinel && static_cast<bool>(std::iswspace(*startPos)))
                        {
                        std::advance(startPos, -1);
                        }
                    }
                functionOrVarNamePos = startPos;
                while (functionOrVarNamePos > startSentinel &&
                       is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                // If we are on the start of the text, then see if we need to include that
                // character too. We may have short circuited because we reached the start
                // of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, 1);
                    }
                variableName.assign(functionOrVarNamePos,
                                    std::next(startPos) - functionOrVarNamePos);

                readVarType();

                if (variableName.length() > 0)
                    {
                    break;
                    }
                }
            else if (static_cast<bool>(std::iswspace(*startPos)))
                {
                std::advance(startPos, -1);
                }
            else if (quoteWrappedInCTOR &&
                     (*startPos == L',' || *startPos == L'+' || *startPos == L'&'))
                {
                quoteWrappedInCTOR = false;
                }
            else if (quoteWrappedInCTOR && *startPos != L',' && *startPos != L'+' &&
                     *startPos != L'&')
                {
                break;
                }
            // << stream operator in some languages.
            // Skip over it and skip over ')' in front of it if there is one
            // to allow things like:
            //     gDebug() << "message"
            else if (*startPos == L'<')
                {
                std::advance(startPos, -1);
                if (startPos > startSentinel && *startPos == L'<')
                    {
                    bool isFunctionCall{ false };
                    std::advance(startPos, -1);
                    while (startPos > startSentinel && static_cast<bool>(std::iswspace(*startPos)))
                        {
                        std::advance(startPos, -1);
                        }
                    // step over arguments to streamable function
                    if (startPos > startSentinel && *startPos == L')')
                        {
                        std::advance(startPos, -1);
                        while (startPos > startSentinel && *startPos != L'(')
                            {
                            std::advance(startPos, -1);
                            }
                        if (startPos > startSentinel)
                            {
                            std::advance(startPos, -1);
                            }
                        isFunctionCall = true;
                        }
                    functionOrVarNamePos = startPos;
                    while (functionOrVarNamePos > startSentinel &&
                           is_valid_name_char_ex(*functionOrVarNamePos))
                        {
                        std::advance(functionOrVarNamePos, -1);
                        }
                    // move back to valid starting character and return
                    if (!is_valid_name_char_ex(*functionOrVarNamePos))
                        {
                        std::advance(functionOrVarNamePos, 1);
                        }
                    if (isFunctionCall)
                        {
                        functionName.assign(functionOrVarNamePos,
                                            std::next(startPos) - functionOrVarNamePos);
                        // ignore localization related functions; in this case, it is the
                        // (temporary) string objects << operator being called,
                        // not the localization function
                        if (m_localization_functions.find(functionName) !=
                                m_localization_functions.cend() ||
                            m_non_localizable_functions.find(functionName) !=
                                m_non_localizable_functions.cend())
                            {
                            functionName.clear();
                            }
                        }
                    else
                        {
                        variableName.assign(functionOrVarNamePos,
                                            std::next(startPos) - functionOrVarNamePos);
                        }

                    return functionOrVarNamePos;
                    }
                }
            else
                {
                if (*startPos == L',')
                    {
                    ++parameterPosition;
                    }
                std::advance(startPos, -1);
                }
            }

        return functionOrVarNamePos;
        }

    //------------------------------------------------
    std::vector<std::wstring>
    i18n_review::convert_positional_cpp_printf(const std::vector<std::wstring>& printfCommands,
                                               std::wstring& errorInfo)
        {
        errorInfo.clear();

        const std::wregex positionalRegex{ L"^%([[:digit:]]+)[$](.*)" };

        std::map<long, std::wstring> positionalCommands;
        std::vector<std::wstring> adjustedCommands{ printfCommands };

        size_t nonPositionalCommands{ 0 };
        for (const auto& pc : printfCommands)
            {
            std::match_results<std::wstring::const_iterator> matches;
            if (std::regex_search(pc, matches, positionalRegex))
                {
                if (matches.size() >= 3)
                    {
                    // position will need to be zero-indexed
                    long position = std::wcstol(matches[1].str().c_str(), nullptr, 10) - 1;
                    const auto [insertionPos, inserted] = positionalCommands.insert(
                        std::make_pair(position, L"%" + matches[2].str()));
                    // if positional argument is used more than once, make sure they are consistent
                    if (!inserted)
                        {
                        if (insertionPos->second != L"%" + matches[2].str())
                            {
#ifdef wxVERSION_NUMBER
                            errorInfo =
                                wxString::Format(_(L"('%s': positional argument provided more than "
                                                   "once, but with different data types.)"),
                                                 matches[0].str())
                                    .wc_str();
#else
                            errorInfo =
                                L"('" + matches[0].str() +
                                _DT(L"': positional argument provided more than once, but with "
                                    "different data types.)");
#endif
                            return std::vector<std::wstring>{};
                            }
                        }
                    };
                }
            else
                {
                ++nonPositionalCommands;
                }
            }

        // Fill output commands from positional arguments.
        // Note that you cannot mix positional and non-positional arguments
        // in the same printf string. If that is happening here, then the
        // non-positional ones will be thrown out and be recorded as an error later.
        if (positionalCommands.size())
            {
            if (nonPositionalCommands > 0)
                {
                errorInfo = _WXTRANS_WSTR(
                    L"(Positional and non-positional commands mixed in the same printf string.)");
                }
            adjustedCommands.clear();
            for (auto& posArg : positionalCommands)
                {
                adjustedCommands.push_back(std::move(posArg.second));
                }
            }

        return adjustedCommands;
        }

    //------------------------------------------------
    std::vector<std::wstring> i18n_review::load_cpp_printf_commands(const std::wstring& resource,
                                                                    std::wstring& errorInfo)
        {
        std::vector<std::pair<size_t, std::wstring>> results;

        // we need to do this multipass because a single regex command for all printf
        // commands is too complex and will cause the regex library to randomly throw exceptions
        std::wstring::const_iterator searchStart(resource.cbegin());
        std::wsmatch res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_int_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_float_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_string_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_pointer_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        std::vector<std::wstring> finalStrings;
        finalStrings.reserve(results.size());
        for (auto& result : results)
            {
            finalStrings.push_back(std::move(result.second));
            }

        return convert_positional_cpp_printf(finalStrings, errorInfo);
        }

    //--------------------------------------------------
    std::vector<std::pair<size_t, size_t>>
    i18n_review::load_file_filter_positions(const std::wstring& resource)
        {
        std::vector<std::pair<size_t, size_t>> results;

        std::wstring::const_iterator searchStart{ resource.cbegin() };
        std::wsmatch res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_file_filter_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.length()));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        return results;
        }

    //--------------------------------------------------
    std::vector<std::pair<size_t, size_t>>
    i18n_review::load_cpp_printf_command_positions(const std::wstring& resource)
        {
        std::vector<std::pair<size_t, size_t>> results;

        // we need to do this multipass because a single regex command for all printf
        // commands is too complex and will cause the regex library to randomly throw exceptions
        std::wstring::const_iterator searchStart{ resource.cbegin() };
        std::wsmatch res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_int_regex))
            {
            // the position and length of the actual specifier, not the junk in front of it
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_float_regex))
            {
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_string_regex))
            {
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_pointer_regex))
            {
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        return results;
        }

    //------------------------------------------------
    std::tuple<bool, std::wstring_view, size_t>
    i18n_review::read_po_catalog_entry(std::wstring_view& poFileText)
        {
        const size_t entryPos = poFileText.find(L"\n#");
        if (entryPos == std::wstring_view::npos)
            {
            return { false, std::wstring_view{}, std::wstring_view::npos };
            }
        poFileText.remove_prefix(entryPos);

        // find the next blank line, which is the separator between catalog entries
        size_t endOfEntryPos{ 0 };
        while (endOfEntryPos != std::wstring_view::npos)
            {
            endOfEntryPos = poFileText.find(L'\n', endOfEntryPos);
            // we must be at the last entry
            if (endOfEntryPos == std::wstring_view::npos ||
                endOfEntryPos == poFileText.length() - 1)
                {
                return { true, poFileText, entryPos };
                }
            ++endOfEntryPos;
            // eat up whitespace on line
            while (endOfEntryPos < poFileText.length() - 1 &&
                   string_util::is_either(poFileText[endOfEntryPos], L'\t', L' '))
                {
                ++endOfEntryPos;
                }
            // stop if we encountered a blank line (with or without empty whitespace in it)
            if (endOfEntryPos == poFileText.length() - 1 ||
                string_util::is_either(poFileText[endOfEntryPos], L'\r', L'\n'))
                {
                break;
                }
            }
        return { true, poFileText.substr(0, endOfEntryPos), entryPos };
        }

    //------------------------------------------------
    std::tuple<bool, std::wstring, size_t, size_t>
    i18n_review::read_po_msg(std::wstring_view& poCatalogEntry, const std::wstring_view msgTag)
        {
        const size_t idPos = poCatalogEntry.find(msgTag);
        if (idPos == std::wstring_view::npos)
            {
            return { false, std::wstring{}, std::wstring::npos, std::wstring::npos };
            }
        // Step back to see if this is a previous translation (#|) or commented
        // out translation (#~).
        size_t lookBehindIndex{ idPos };
        while (lookBehindIndex > 0 &&
               string_util::is_neither(poCatalogEntry[lookBehindIndex], L'\r', L'\n'))
            {
            --lookBehindIndex;
            }
        if (poCatalogEntry[++lookBehindIndex] == L'#')
            {
            return { false, std::wstring{}, std::wstring::npos, std::wstring::npos };
            }

        poCatalogEntry.remove_prefix(idPos + msgTag.length());

        size_t idEndPos{ 0 };
        while (true)
            {
            idEndPos = poCatalogEntry.find(L'\"', idEndPos);
            if (idEndPos == std::wstring_view::npos)
                {
                return { false, std::wstring{}, std::wstring::npos, std::wstring::npos };
                }
            // skip escaped quotes
            if (idEndPos > 0 && poCatalogEntry[idEndPos - 1] == L'\\')
                {
                ++idEndPos;
                continue;
                }
            else
                {
                size_t lookAheadIndex{ idEndPos + 1 };
                // jump to next line
                while (lookAheadIndex < poCatalogEntry.length() &&
                       string_util::is_either(poCatalogEntry[lookAheadIndex], L'\r', L'\n'))
                    {
                    ++lookAheadIndex;
                    }
                // eat up leading spaces
                while (lookAheadIndex < poCatalogEntry.length() &&
                       string_util::is_either(poCatalogEntry[lookAheadIndex], L'\t', L' '))
                    {
                    ++lookAheadIndex;
                    }
                // if a quote, then this is still be part of the same string
                if (lookAheadIndex < poCatalogEntry.length() &&
                    poCatalogEntry[lookAheadIndex] == L'"')
                    {
                    idEndPos = lookAheadIndex + 1;
                    continue;
                    }
                break;
                }
            }
        const std::wstring msgId{ process_po_msg(poCatalogEntry.substr(0, idEndPos)) };

        poCatalogEntry.remove_prefix(idEndPos);

        return { true, msgId, idPos, idEndPos };
        }

    //--------------------------------------------------
    std::pair<size_t, size_t>
    i18n_review::get_line_and_column(size_t position,
                                     std::wstring_view fileStart /*= std::wstring_view*/) const
        {
        if (position == std::wstring::npos)
            {
            return std::make_pair(std::wstring::npos, std::wstring::npos);
            }

        const auto* startSentinel = (!fileStart.empty() ? fileStart.data() : m_file_start);
        if (startSentinel == nullptr)
            {
            return std::make_pair(std::wstring::npos, std::wstring::npos);
            }
        size_t nextLinePosition{ 0 };
        size_t lineCount{ 0 };
        while ((nextLinePosition = std::wcscspn(startSentinel, L"\r\n")) < position)
            {
            ++lineCount;
            if (nextLinePosition + 1 < position &&
                *std::next(startSentinel, static_cast<ptrdiff_t>(nextLinePosition)) == L'\r' &&
                *std::next(startSentinel, static_cast<ptrdiff_t>(nextLinePosition + 1)) == L'\n')
                {
                std::advance(startSentinel, nextLinePosition + 2);
                position -= nextLinePosition + 2;
                }
            else
                {
                std::advance(startSentinel, nextLinePosition + 1);
                position -= nextLinePosition + 1;
                }
            }
        // make one-indexed
        return std::make_pair(lineCount + 1, position + 1);
        }
    } // namespace i18n_check
