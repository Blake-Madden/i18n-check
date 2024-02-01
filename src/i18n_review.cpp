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
    // common font faces that we would usually ignore (client can add to this)
    std::set<string_util::case_insensitive_wstring> i18n_review::m_font_names = { // NOLINT
        L"Arial",
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
    std::set<string_util::case_insensitive_wstring> i18n_review::m_file_extensions = {  // NOLINT
        L"xml", L"html", L"htm", L"xhtml", L"rtf", L"doc", L"docx", L"dot", L"docm", L"txt", L"ppt",
        L"pptx", L"pdf", L"ps", L"odt", L"ott", L"odp", L"otp", L"pptm", L"md", L"xaml",
        // Visual Studio files
        L"sln", L"csproj", L"json", L"pbxproj", L"apk", L"tlb", L"ocx", L"pdb", L"tlh", L"hlp",
        L"msi", L"rc", L"vcxproj",
        // macOS
        L"dmg", L"proj", L"xbuild", L"xmlns",
        // Database
        L"mdb", L"db",
        // Markdown files
        L"md", L"Rmd", L"qmd",
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
        L"cpp", L"h", L"c", L"idl", L"cs",
        // compressed files
        L"gzip", L"bz2"
    };

    //--------------------------------------------------
    i18n_review::i18n_review()
        {
        m_deprecated_string_macros = {
            { L"wxT", L"Deprecated text macro can be removed. "
                      "(Add 'L' in front of string to make it double-byte.)" },
            { L"wxT_2", L"Deprecated text macro can be removed. "
                        "(Add 'L' in front of string to make it double-byte.)" },
            { L"wxS", L"Deprecated text macro can be removed. "
                      "(Add 'L' in front of string to make it double-byte.)" },
            { L"_T", L"Deprecated text macro can be removed. "
                     "(Add 'L' in front of string to make it double-byte.)" },
            { L"TEXT", L"Deprecated text macro can be removed. "
                       "(Add 'L' in front of string to make it double-byte.)" },
            { L"_TEXT", L"Deprecated text macro can be removed. "
                        "(Add 'L' in front of string to make it double-byte.)" },
            { L"__TEXT", L"Deprecated text macro can be removed. "
                         "(Add 'L' in front of string to make it double-byte.)" },
            { L"_WIDE", L"Deprecated text macro can be removed. "
                        "(Add 'L' in front of string to make it double-byte.)" }
        };

        // Whole file needs to be scanned for these, as string variables can be passed to these
        // as well as hard-coded strings.
        m_deprecated_string_functions = {
            // Win32 TCHAR functions (which mapped between _MBCS and _UNICODE builds).
            // Nowadays, you should be using _UNICODE (i.e., UTF-16) always.
            { L"_tcscat", L"Use std::wcscat() instead." },
            { L"_tcschr", L"Use std::wcschr() instead." },
            { L"_tcscmp", L"Use std::wcscmp() instead." },
            { L"_tcscoll", L"Use std::wcscoll() instead." },
            { L"_tcscpy", L"Use std::wcscpy() instead." },
            { L"_tcsdup", L"Use std::wcsdup() instead." },
            { L"_tcslen", L"Use std::wcslen() or (wrap in a std::wstring_view) instead." },
            { L"_tcsncat", L"Use std::wcsncat() instead." },
            { L"_tcsnccmp", L"Use std::wcsncmp() instead." },
            { L"_tcsncpy", L"Use std::wcsncpy() instead." },
            { L"_tcspbrk", L"Use std::wcspbrk() instead." },
            { L"_tcsrchr", L"Use std::wcsrchr() instead." },
            { L"_tcsspn", L"Use std::wcsspn() instead." },
            { L"_tcsstr", L"Use std::wcsstr() instead." },
            { L"_tcstok", L"Use std::wcstok() instead." },
            { L"_tcsxfrm", L"Use std::wcsxfrm() instead." },
            { L"wsprintf", L"Use std::swprintf() instead." },
            { L"_stprintf", L"Use std::swprintf() instead." },
            { L"TCHAR", L"Use wchar_t instead." },
            // wxWidgets
            { L"wxStrlen", L"Use std::wcslen() or (wrap in a std::wstring_view) instead." },
            { L"wxStrstr", L"Use std::wcsstr() instead." },
            { L"wxStrchr", L"Use std::wcschr() instead." },
            { L"wxStrdup", L"Use std::wcsdup() instead." },
            { L"wxStrcpy", L"Use std::wcscpy() instead." },
            { L"wxStrncpy", L"Use std::wcsncpy() instead." },
            { L"wxStrcat ", L"Use std::wcscat() instead." },
            { L"wxStrncat", L"Use std::wcsncat() instead." },
            { L"wxStrtok", L"Use std::wcstok() instead." },
            { L"wxStrrchr", L"Use std::wcsrchr() instead." },
            { L"wxStrpbrk", L"Use std::wcspbrk() instead." },
            { L"wxStrxfrm", L"Use std::wcsxfrm() instead." },
            { L"wxIsEmpty", L"Use wxString's empty() member instead." },
            { L"wxIsdigit", L"Use std::iswdigit() instead." },
            { L"wxIsalnum", L"Use std::iswalnum() instead." },
            { L"wxIsalpha", L"Use std::iswalpha() instead." },
            { L"wxIsctrl", L"Use std::iswctrl() instead." },
            { L"wxIspunct", L"Use std::iswpunct() instead." },
            { L"wxIsspace", L"Use std::iswpspace() instead." },
            { L"wxChar", L"Use wchar_t instead." },
            { L"wxStrftime", L"Use wxDateTime's formatting functions instead." },
            { L"wxStrtod", L"Use wxString::ToDouble() instead." },
            { L"wxTrace", L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                          L"functions instead." },
            { L"WXTRACE", L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                          L"functions instead." },
            { L"wxTraceLevel", L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                               L"functions instead." },
            { L"wxUnix2DosFilename", L"Construct a wxFileName with wxPATH_UNIX and then use "
                                     "wxFileName::GetFullPath(wxPATH_DOS) instead." },
            { L"wxSplitPath",
              L"This function is obsolete, please use wxFileName::SplitPath() instead." },
            // not i18n related, just legacy wx functions that can be modernized after c++11
            { L"wxMin", L"Use std::min() instead." },
            { L"wxMax", L"Use std::max() instead." },
            { L"wxRound", L"Use std::lround() instead." },
            { L"wxIsNan", L"Use std::isnan() instead." },
            { L"__WXMAC__", L"Use __WXOSX__ instead." },
            { L"wxOVERRIDE", L"Use override or final instead." },
            { L"wxNOEXCEPT", L"Use noexcept instead (requires C++17)." },
            { L"WXUNUSED", L"Use [[maybe_unused]] instead (requires C++17)." },
            { L"wxDECLARE_NO_COPY_CLASS",
              L"Delete the copy CTOR and assignment operator instead." }
        };

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
            std::wregex(LR"(([A-Z]+|[bB]itmap) [(]([*][.][A-Za-z0-9]{1,5}[)]))"),
            std::wregex(
                LR"((([A-Z]+|[bB]itmap) [(]([*][.][A-Za-z0-9]{1,5})(;[*][.][A-Za-z0-9]{1,5})*[)][|]([*][.][A-Za-z0-9]{1,5})(;[*][.][A-Za-z0-9]{1,5})*[|]{0,2})+)"),
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
            // Pascal-case words (e.g., "GetValueFromUser"); surrounding punctuation is stripped
            // first.
            std::wregex(LR"([[:punct:]]*[A-Z]+[a-z0-9]+([A-Z]+[a-z0-9]+)+[[:punct:]]*)"),
            // camel-case words (e.g., "getValueFromUser"); surrounding punctuation is stripped
            // first.
            std::wregex(LR"([[:punct:]]*[a-z]+[[:digit:]]*([A-Z]+[a-z0-9]+)+[[:punct:]]*)"),
            // formulas (e.g., ABS(-2.7), POW(-4, 2), =SUM(1; 2) )
            std::wregex(LR"((=)?[A-Za-z0-9_]{3,}[(]([RC0-9\-\.,;:\[\] ])*[)])"),
            // formulas (e.g., ComputeNumbers() )
            std::wregex(LR"([A-Za-z0-9_]{3,}[(][)])"),
            // equal sign followed by a single word is probably some sort of config file tag or
            // formula.
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
            std::wregex(LR"(^DEBUG:[\s\S]*)"),
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
            // products
            std::wregex(LR"((Microsoft )?Visual Studio)"),
            std::wregex(LR"((Microsoft )?Visual C\+\+)"),
            std::wregex(LR"((Microsoft )?Visual Basic)")
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
            // Qt (note that NOOP functions actually do load something for translation, just not
            // in-place)
            L"tr", L"trUtf8", L"translate", L"QT_TR_NOOP", L"QT_TRANSLATE_NOOP",
            // KDE (ki18n)
            L"i18n", L"i18np", L"i18ncp", L"i18nc", L"xi18n", L"xi18nc"
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
        //      function/variable assignment to the left of these).
        m_ctors_to_ignore = {
            // Win32 text macros that should be skipped over
            L"_T", L"TEXT", L"_TEXT", L"__TEXT", L"_WIDE",
            // macOS
            L"CFSTR", L"CFStringRef",
            // similar macros from other libraries
            L"T",
            // wxWidgets
            L"wxT", L"wxT_2", L"wxS", L"wxString", L"wxBasicString", L"wxCFStringRef",
            L"wxASCII_STR",
            // Qt
            L"QString", L"QLatin1String", L"QStringLiteral", L"setStyleSheet",
            // standard string objects
            L"basic_string", L"string", L"wstring", L"u8string", L"u16string", L"u32string",
            L"std::basic_string", L"std::string", L"std::wstring", L"std::u8string",
            L"std::u16string", L"std::u32string", L"std::pmr::basic_string", L"std::pmr::string",
            L"std::pmr::wstring", L"std::pmr::u8string", L"std::pmr::u16string",
            L"std::pmr::u32string", L"pmr::basic_string", L"pmr::string", L"pmr::wstring",
            L"pmr::u8string", L"pmr::u16string", L"pmr::u32string",
            // MFC, ATL
            L"CString", L"_bstr_t",
            // formatting functions (not actually a CTOR) that should be skipped over
            L"wxString::Format"
        };

        // debugging & system call functions that should never have
        // their string parameters translated
        m_internal_functions = {
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
            L"wxGetCommandOutput", L"SetKeyWords",
            // Qt
            L"Q_ASSERT", L"Q_ASSERT_X", L"qSetMessagePattern", L"qmlRegisterUncreatableMetaObject",
            L"addShaderFromSourceCode", L"QStandardPaths::findExecutable", L"QDateTime::fromString",
            L"qCDebug", L"qDebug",
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
            // other testing frameworks
            L"do_test", L"run_check", L"GNC_TEST_ADD_FUNC", L"GNC_TEST_ADD", L"g_test_message",
            L"check_binary_op", L"check_binary_op_equal", L"MockProvider",
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
            L"CFBundleCopyResourceURL",
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
            L"SetRegistryKey",
            // zlib
            L"Tracev", L"Trace", L"Tracevv",
            // Lua
            L"luaL_error", L"lua_pushstring", L"lua_setglobal",
            // more functions from various apps
            L"trace", L"ActionFormat", L"ErrorFormat", L"addPositionalArgument", L"DEBUG"
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
            L"PERR", L"PWARN", L"PINFO", L"ENTER", L"LEAVE"
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
                                     L"32 bit", L"32-bit", L"64 bit", L"64-bit",
                                     // RTF font families
                                     L"fnil", L"fdecor", L"froman", L"fscript", L"fswiss",
                                     L"fmodern", L"ftech",
                                     // common UNIX names (Windows versions are handled by more
                                     // complex regex expressions elsewhere)
                                     L"UNIX", L"macOS", L"Apple Mac OS", L"Apple Mac OS X", L"OSX",
                                     L"Linux", L"FreeBSD", L"POSIX", L"NetBSD" };

        // keywords in the language that can appear in front of a string only
        m_keywords = { L"return", L"else", L"if", L"goto", L"new", L"delete", L"throw" };

        // variables whose CTORs take a string that should never be translated
        m_variable_types_to_ignore = { L"wxUxThemeHandle",
                                       L"wxRegKey",
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
                                       L"wxRegEx",
                                       L"wregex",
                                       L"std::wregex",
                                       L"regex",
                                       L"std::regex",
                                       L"wxDataObjectSimple" };

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
        add_variable_name_pattern_to_ignore(std::wregex(LR"(wxColourDialogNames)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(wxColourTable)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(QT_MESSAGE_PATTERN)"));
        }

    //--------------------------------------------------
    void i18n_review::load_deprecated_functions(const std::wstring_view fileText,
                                                const std::wstring& fileName)
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
                                          const std::wstring& fileName)
        {
        if (!(static_cast<bool>(m_reviewStyles & check_duplicate_value_assigned_to_ids) ||
              static_cast<bool>(m_reviewStyles & check_number_assigned_to_id)))
            {
            return;
            }
        std::vector<std::wstring> matches;
        std::copy(std::regex_token_iterator<decltype(fileText)::const_iterator>(
                      fileText.cbegin(), fileText.cend(), m_id_assignment_regex),
                  std::regex_token_iterator<decltype(fileText)::const_iterator>{},
                  std::back_inserter(matches));

        const std::wregex varNamePartsRE{ L"([a-zA-Z0-9_]*)(ID)([a-zA-Z0-9_]*)" };
        const std::wregex varNameIDPartsRE{ L"([a-zA-Z0-9_]*)(ID[A-Z]?[_]?)([a-zA-Z0-9_]*)" };
        // no std::from_chars for wchar_t :(
        const std::wregex numRE{ LR"(^[\-0-9']+$)" };
        if (!matches.empty())
            {
            std::vector<std::pair<std::wstring, std::wstring>> idAssignments;
            idAssignments.reserve(matches.size());
            std::vector<std::wstring> subMatches;
            std::vector<std::wstring> idNameParts;
            // ID and variable name assigned to
            std::map<std::wstring, std::wstring> assignedIds;
            for (const auto& match : matches)
                {
                subMatches.clear();
                idNameParts.clear();
                // get the var name and ID
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(match)>::const_iterator>(
                              match.cbegin(), match.cend(), m_id_assignment_regex, { 3, 4, 5 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(match)>::const_iterator>{},
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
                    idAssignments.push_back(std::make_pair(subMatches[0], subMatches[1]));
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

                idAssignments.push_back(std::make_pair(subMatches[0], subMatches[1]));
                }
            for (const auto& idAssignment : idAssignments)
                {
                idNameParts.clear();
                std::copy(
                    std::regex_token_iterator<
                        std::remove_reference_t<decltype(idAssignment.first)>::const_iterator>(
                        idAssignment.first.cbegin(), idAssignment.first.cend(), varNameIDPartsRE,
                        { 1, 2, 3 }),
                    std::regex_token_iterator<
                        std::remove_reference_t<decltype(idAssignment.first)>::const_iterator>(),
                    std::back_inserter(idNameParts));
                const auto idVal = [&idAssignment]()
                {
                    try
                        {
                        return std::optional<int32_t>(std::stol(idAssignment.second));
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
                        idAssignment.second + L" assigned to " + idAssignment.first +
                            L"; value should be between 1 and 0x6FFF if this is an MFC project.",
                        string_info::usage_info{}, fileName,
                        std::make_pair(std::wstring::npos, std::wstring::npos)));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_number_assigned_to_id) && idVal &&
                         !(idVal.value() >= idRangeStart && idVal.value() <= stringIdRangeEnd) &&
                         (idNameParts[1] == L"IDS_" || idNameParts[1] == L"IDP_"))
                    {
                    m_ids_assigned_number.emplace_back(
                        idAssignment.second + L" assigned to " + idAssignment.first +
                            L"; value should be between 1 and 0x7FFF if this is an MFC project.",
                        string_info::usage_info{}, fileName,
                        std::make_pair(std::wstring::npos, std::wstring::npos));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_number_assigned_to_id) && idVal &&
                         !(idVal.value() >= dialogIdRangeStart &&
                           idVal.value() <= dialogIdRangeEnd) &&
                         idNameParts[1] == L"IDC_")
                    {
                    m_ids_assigned_number.emplace_back(
                        idAssignment.second + L" assigned to " + idAssignment.first +
                            L"; value should be between 8 and 0xDFFF if this is an MFC project.",
                        string_info::usage_info{}, fileName,
                        std::make_pair(std::wstring::npos, std::wstring::npos));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_number_assigned_to_id) &&
                         idNameParts[1].length() <= 3 && // ignore MFC IDs (handled above)
                         std::regex_match(idAssignment.second, numRE) &&
                         // -1 or 0 are usually generic IDs for the framework or temporary init
                         // values
                         idAssignment.second != L"-1" && idAssignment.second != L"0")
                    {
                    m_ids_assigned_number.emplace_back(
                        idAssignment.second + L" assigned to " + idAssignment.first,
                        string_info::usage_info{}, fileName,
                        std::make_pair(std::wstring::npos, std::wstring::npos));
                    }
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26478)
#endif
                const auto [pos, inserted] =
                    assignedIds.insert(std::make_pair(idAssignment.second, idAssignment.first));
#ifdef _MSC_VER
#pragma warning(pop)
#endif
                if (static_cast<bool>(m_reviewStyles & check_duplicate_value_assigned_to_ids) &&
                    !inserted && idAssignment.second.length() > 0 &&
                    // ignore if same ID is assigned to variables with the same name
                    idAssignment.first != pos->second && idAssignment.second != L"wxID_ANY" &&
                    idAssignment.second != L"wxID_NONE" && idAssignment.second != L"-1" &&
                    idAssignment.second != L"0")
                    {
                    m_duplicates_value_assigned_to_ids.push_back(string_info(
                        idAssignment.second + L" has been assigned to multiple ID variables.",
                        string_info::usage_info{}, fileName,
                        std::make_pair(std::wstring::npos, std::wstring::npos)));
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
                    (functionName == L"translate" && parameterPosition == 0) ||
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
        clear_section(currentTextPos, quoteEnd + 1);
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
        // one message per file is more than reasonable
        m_error_log.reserve(fileCount);
        constexpr size_t resourcesPerFile = 100;
        m_localizable_strings.reserve(resourcesPerFile * fileCount);
        m_not_available_for_localization_strings.reserve(resourcesPerFile * fileCount);
        m_marked_as_non_localizable_strings.reserve(resourcesPerFile * fileCount);
        m_internal_strings.reserve(resourcesPerFile * fileCount);
        m_unsafe_localizable_strings.reserve(resourcesPerFile * fileCount);
        m_deprecated_macros.reserve(resourcesPerFile * fileCount);
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
    bool i18n_review::is_untranslatable_string(std::wstring& str, const bool limitWordCount) const
        {
        static const std::wregex oneWordRE{ LR"((\b[a-zA-Z'\-]+([\.\-\/:]*[\w'\-]*)*))" };
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
            if (std::regex_match(str, m_html_regex) ||
                std::regex_match(str, m_html_element_regex) ||
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

            // "N/A" is OK to translate, but it won't meet the criterion of at least
            // two consecutive letters, so check for that first.
            if (str.length() == 3 && string_util::is_either(str[0], L'N', L'n') && str[1] == L'/' &&
                string_util::is_either(str[2], L'A', L'a'))
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
                m_known_internal_strings.find(str.c_str()) != m_known_internal_strings.end())
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
            if (is_font_name(str.c_str()) || is_file_extension(str.c_str()) ||
                i18n_string_util::is_file_address(str))
                {
                return true;
                }
            // nothing seems to match, probably a real string
            return false;
            }
        catch (const std::exception& exp)
            {
            log_message(str, i18n_string_util::lazy_string_to_wstring(exp.what()),
                        std::wstring::npos);
            return false;
            }
        }

    //--------------------------------------------------
    std::wstring i18n_review::collapse_multipart_string(const std::wstring& str)
        {
        // leave raw strings as-is
        if (str.length() >= 2 && str.front() == L'(' && str.back() == L')')
            {
            return str;
            }
        const std::wregex reg(LR"(([^\\])("[\s]*"))");
        return std::regex_replace(str, reg, L"$1");
        }

    //--------------------------------------------------
    void i18n_review::process_strings()
        {
        const auto processStrings = [](auto& strings)
        {
            std::for_each(strings.begin(), strings.end(),
                          [](auto& val)
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
                const auto* typeEnd = functionOrVarNamePos + 1;
                // if a template, then step over (going backwards) the template arguments
                // to get to the root type
                if (typeEnd - 1 > startSentinel && typeEnd[-1] == L'>')
                    {
                    // if a pointer accessor (->) then bail as it won't be a variable assignment
                    if (typeEnd - 2 > startSentinel && typeEnd[-2] == L'-')
                        {
                        return;
                        }
                    assert(functionOrVarNamePos >= startSentinel);
                    const auto openingAngle = string_util::find_last_of(
                        startSentinel, L'<', functionOrVarNamePos - startSentinel);
                    if (openingAngle == std::wstring::npos)
                        {
                        log_message(L"Template parse error",
                                    L"Unable to find opening < for template variable.",
                                    functionOrVarNamePos - startSentinel);
                        return;
                        }
                    functionOrVarNamePos = startSentinel + openingAngle;
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
                functionName.assign(functionOrVarNamePos, (startPos + 1) - functionOrVarNamePos);
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
            else if (*startPos == L'=' && startPos[1] != L'=' && startPos > startSentinel &&
                     *(startPos - 1) != L'=' && *(startPos - 1) != L'!' &&
                     *(startPos - 1) != L'>' && *(startPos - 1) != L'<')
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
                variableName.assign(functionOrVarNamePos, (startPos + 1) - functionOrVarNamePos);

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
            // << stream operator in some languages,
            // skip over it and skip over ')' in front of it if there is one
            // to allow things like:
            //     gDebug() << "message"
            else if (*startPos == L'<')
                {
                std::advance(startPos, -1);
                if (startPos > startSentinel && *startPos == L'<')
                    {
                    std::advance(startPos, -1);
                    while (startPos > startSentinel && static_cast<bool>(std::iswspace(*startPos)))
                        {
                        std::advance(startPos, -1);
                        }
                    if (startPos > startSentinel && *startPos == L')')
                        {
                        std::advance(startPos, -1);
                        }
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

    //--------------------------------------------------
    std::pair<size_t, size_t>
    i18n_review::get_line_and_column(size_t position,
                                     const wchar_t* fileStart /*= nullptr*/) const noexcept
        {
        if (position == std::wstring::npos)
            {
            return std::make_pair(std::wstring::npos, std::wstring::npos);
            }

        const auto* startSentinel = ((fileStart != nullptr) ? fileStart : m_file_start);
        if (startSentinel == nullptr)
            {
            return std::make_pair(std::wstring::npos, std::wstring::npos);
            }
        size_t nextLinePosition{ 0 };
        size_t lineCount{ 0 };
        while ((nextLinePosition = std::wcscspn(startSentinel, L"\r\n")) < position)
            {
            ++lineCount;
            if (nextLinePosition + 1 < position && startSentinel[nextLinePosition] == L'\r' &&
                startSentinel[nextLinePosition + 1] == L'\n')
                {
                startSentinel += nextLinePosition + 2;
                position -= nextLinePosition + 2;
                }
            else
                {
                startSentinel += nextLinePosition + 1;
                position -= nextLinePosition + 1;
                }
            }
        // make one-indexed
        return std::make_pair(lineCount + 1, position + 1);
        }
    } // namespace i18n_check
