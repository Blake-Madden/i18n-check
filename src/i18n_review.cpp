///////////////////////////////////////////////////////////////////////////////
// Name:        i18n_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2023 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "i18n_review.h"

using namespace i18n_string_util;

namespace i18n_check
    {
    //--------------------------------------------------
    i18n_review::i18n_review() :
        // HTML, but also includes some GTK formatting tags
        m_html_regex(L"[^[:alnum:]<]*<(span|object|property|div|p|ul|ol|li|img|html|[?]xml|meta|body|table|tbody|tr|td|thead|head|title|a[[:space:]]|!--|/|!DOCTYPE|br|center|dd|em|dl|dt|tt|font|form|h[[:digit:]]|hr|main|map|pre|script).*",
            std::regex_constants::icase),
        // <doc-val>Some text</doc-val>
        m_html_element_regex(L"<[a-zA-Z0-9_\\-]+>[[:print:][:cntrl:]]*</[a-zA-Z0-9_\\-]+>",
            std::regex_constants::icase),
        m_html_tag_regex(L"&[[:alpha:]]{2,5};.*"),
        m_html_tag_unicode_regex(L"&#[[:digit:]]{2,4};.*"),
        m_2letter_regex(L"[[:alpha:]]{2,}"),
        m_hashtag_regex(L"#[[:alnum:]]{2,}"),
        m_key_shortcut_regex(L"(CTRL|SHIFT|CMD|ALT)([+](CTRL|SHIFT|CMD|ALT))*([+][[:alnum:]])+",
            std::regex_constants::icase),
        m_function_signature_regex(L"[[:alnum:]]{2,}[(][[:alnum:]]+(,[[:space:]]*[[:alnum:]]+)*[)]"),
        m_plural_regex(L"[[:alnum:]]{2,}[(]s[)]"),
        m_open_function_signature_regex(L"[[:alnum:]]{2,}[(]"),
        m_diagnostsic_function_regex(L"([a-zA-Z0-9_]*|^)(ASSERT|VERIFY|PROFILE|CHECK)([a-zA-Z0-9_]*|$)")
        {
        m_deprecated_string_macros = {
            { L"wxT", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"wxT_2", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"wxS", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"_T", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"TEXT", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"_TEXT", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"__TEXT", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"_WIDE", L"Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.)" },
            { L"wxStrlen", L"Consider using std::wcslen() or (wrap in a std::wstring_view) instead." },
            { L"wxTrace", L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() functions instead." },
            { L"WXTRACE", L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() functions instead." },
            { L"wxTraceLevel", L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() functions instead." },
            { L"wxUnix2DosFilename", L"Construct a wxFileName with wxPATH_UNIX and then use wxFileName::GetFullPath(wxPATH_DOS) instead." },
            { L"wxSplitPath", L"This function is obsolete, please use wxFileName::SplitPath() instead." }
        };

        m_untranslatable_regexes = {
            // nothing but numbers, punctuation, control characters?
            std::wregex(L"([[:digit:][:space:][:punct:][:cntrl:]]|\\\\[rnt])+"),
            // placeholder text
            std::wregex(L"Lorem ipsum.*"),
            // SQL code
            std::wregex(L"(INSERT INTO|SELECT [*])[[:space:]].*",
                std::regex_constants::icase),
            // a regex expression
            std::wregex(L"[(][?]i[)].*"),
            // single file filter that just has a file extension as its "name"
            // PNG (*.png)|*.png
            // TIFF (*.tif;*.tiff)|*.tif;*.tiff
            // special case for the word "bitmap" also, wouldn't normally translate that
            std::wregex(
                L"([A-Z]+|[bB]itmap) [(]([*][.][[:alnum:]]{1,5})(;[*][.][[:alnum:]]{1,5})*[)][|]"
                 "([*][.][[:alnum:]]{1,5})(;[*][.][[:alnum:]]{1,5})*"),
            // generic measuring string (or regex expression)
            std::wregex(L"[[:space:]]*(ABCDEFG|abcdefg).*"),
            // debug messages
            std::wregex(L"Assert(ion)? (f|F)ail.*"),
            std::wregex(L"ASSERT *"),
            // HTML entities
            std::wregex(L"&[#]?[xX]?[[:alnum:]]+;"),
            // An opening HTML element
            std::wregex(L"<(body|html|img|head|meta|style|span|p|tr|td)"),
            // PostScript element
            std::wregex(L"%%[[:alpha:]]+:.*"),
            // XML elements
            std::wregex(LR"(<([A-Za-z])+([A-Za-z0-9_/\\\-\.'"=;:#[:space:]])+[>]?)"),
            std::wregex(LR"(xml[ ]*version[ ]*=[ ]*\\["'][0-9\.]+\\["'][>]?)"),
            std::wregex(LR"(<[A-Za-z]+[A-Za-z0-9_/\\\-\.'"=;:[:space:]]+>[[:space:][:digit:][:punct:]]*<[A-Za-z0-9_/\-.']*>)"),
            std::wregex(LR"(<[A-Za-z]+([A-Za-z0-9_\-\.]+[[:space:]]*){1,2}=[[:punct:]A-Za-z0-9]*)"),
            // <image x=%d y=\"%d\" width = '%dpx' height="%dpx"
            std::wregex(L"<[A-Za-z0-9_\\-\\.]+[[:space:]]*([A-Za-z0-9_\\-\\.]+[[:space:]]*=[[:space:]]*[\"'\\\\]{0,2}[a-zA-Z0-9\\-]*[\"'\\\\]{0,2}[[:space:]]*)+"),
            std::wregex(L"charset[[:space:]]*=.*",
                std::regex_constants::icase),
            // all 'X'es and spaces, usually a placeholder of some sort
            std::wregex(L"[xX ]+"),
            // bash command (e.g., "lpstat -p") and system variables
            std::wregex(L"[[:alpha:]]{3,} [\\-][[:alpha:]]+"),
            std::wregex(L"sys[$].*"),
            // Pascal-case words (e.g., "GetValueFromUser"); surrounding punctuation is stripped first.
            std::wregex(L"[[:punct:]]*[A-Z]+[a-z0-9]+([A-Z]+[a-z0-9]+)+[[:punct:]]*"),
            // camel-case words (e.g., "getValueFromUser"); surrounding punctuation is stripped first.
            std::wregex(L"[[:punct:]]*[a-z]+[[:digit:]]*([A-Z]+[a-z0-9]+)+[[:punct:]]*"),
            // formulas (e.g., ABS(-2.7), POW(-4, 2), =SUM(1; 2) )
            std::wregex(LR"((=)?[A-Za-z0-9_]{3,}[(]([RC0-9\-\.,;:\[\] ])*[)])"),
            // formulas (e.g., ComputeNumbers() )
            std::wregex(L"[A-Za-z0-9_]{3,}[(][)]"),
            // equal sign followed by a single word is probably some sort of config file tag or formula.
            std::wregex(L"=[A-Za-z0-9_]+"),
            // character encodings
            std::wregex(L"(utf[-]?[[:digit:]]+|Shift[-_]JIS|us-ascii|windows-[[:digit:]]{4}|KOI8-R|Big5|GB2312|iso-[[:digit:]]{4}-[[:digit:]]+)",
                std::regex_constants::icase),
            // wxWidgets constants
            std::wregex(L"(wx|WX)[A-Z_0-9]{2,}"),
            // ODCTask --surrounding punctuation is stripped first
            std::wregex(L"[[:punct:]]*[A-Z]{3,}[a-z_0-9]{2,}[[:punct:]]*"),
            // Config_File, "user_level_permission", "__HIGH_SCORE__"
            std::wregex(L"[_]*[a-zA-Z0-9]+(_[a-zA-Z0-9]+)+[_]*"),
            // CSS strings
            std::wregex(L"font-(style|weight|family|size|face-name|underline|point-size)[[:space:]]*[:]?.*",
                std::regex_constants::icase),
            std::wregex(L"text-decoration[[:space:]]*[:]?.*", std::regex_constants::icase),
            std::wregex(L"(background-)?color[[:space:]]*:.*", std::regex_constants::icase),
            std::wregex(L"style[[:space:]]*=[\"']?.*", std::regex_constants::icase),
            // local file paths & file names
            std::wregex(L"(WINDIR|Win32|System32|Kernel32|/etc|/tmp)", std::regex_constants::icase),
            std::wregex(L"(so|dll|exe|dylib|jpg|bmp|png|gif|txt|doc)",
                std::regex_constants::icase), // common file extension that might be missing the period
            std::wregex(L"[.][a-zA-Z0-9]{1,5}"), // file extension
            std::wregex(L"[.]DS_Store"), // macOS file
            std::wregex(L"[\\\\/]?[[:alnum:]_~!@#$%&;',+={}().^\\[\\]\\-]+([.][a-zA-Z0-9]{1,4})+"), // file name (supports multiple extensions)
            std::wregex(L"([[:alnum:]_-]+[\\\\/]){1,2}[[:alnum:]_-]+([.][a-zA-Z0-9]{1,4})+"), // ultra simple relative file path (e.g., "shaders/player1.vert")
            std::wregex(L"\\*[.][a-zA-Z0-9]{1,5}"), // wild card file extension
            std::wregex(L"([/]{1,2}[[:alnum:]_~!@#$%&;',+={}().^\\[\\]\\-]+){2,}/?"), // UNIX or web folder (needs at least 1 folder in path)
            std::wregex(L"[a-zA-Z][:]([\\\\]{1,2}[[:alnum:]_~!@#$%&;',+={}().^\\[\\]\\-]*)+"), // Windows folder
            std::wregex(L"[/]?sys\\$.*"),
            // URL
            std::wregex(LR"(((http|ftp)s?:\/\/)?(www\.)[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))"),
            // email address
            std::wregex(LR"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$)"),
            std::wregex(LR"(^[\w ]*<[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*>$)"),
            // Windows HTML clipboard data
            std::wregex(L".*(End|Start)(HTML|Fragment)[:]?[[:digit:]]*.*"),
            // printer commands (e.g., @PAGECOUNT@)
            std::wregex(L"@[A-Z0-9]+@"),
            // [CMD]
            std::wregex(L"\\[[A-Z0-9]+\\]"),
            // Windows OS names
            std::wregex(L"(Microsoft )?Windows (95|98|NT|ME|2000|Server|Vista|Longhorn|XP|[[:digit:]]{1,2}[.]?[[:digit:]]{0,2})[[:space:]]*[[:digit:]]{0,4}[[:space:]]*(R|SP)?[[:digit:]]{0,2}")
            };

        // functions/macros that indicate that a string will be localizable
        m_localization_functions = {
            // GNU's gettext C/C++ functions
            L"_", L"gettext", L"dgettext", L"ngettext", L"dngettext",
            L"pgettext", L"dpgettext", L"npgettext", L"dnpgettext", L"dcgettext",
            // GNU's propername module
            L"proper_name", L"proper_name_utf8",
            // wxWidgets gettext wrapper functions
            // (*_IN_CONTEXT macros are not included as they take string parameters as keys)
            L"wxPLURAL", L"wxTRANSLATE", L"wxGetTranslation"
            };

        // functions that indicate that a string is explicitly marked to not be translatable
        m_non_localizable_functions = { L"_DT", L"DONTTRANSLATE",
            // these are not defined explicitly in gettext, but their documenation suggests
            // that you can add them as defines in your code and use them
            L"gettext_noop", L"N_" };

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
            L"wxASCII_STR",
            // Qt
            L"QString",
            // standard string objects
            L"basic_string", L"string", L"wstring", L"u8string", L"u16string", L"u32string",
            L"std::basic_string", L"std::string", L"std::wstring", L"std::u8string",
            L"std::u16string", L"std::u32string",
            L"std::pmr::basic_string", L"std::pmr::string", L"std::pmr::wstring",
            L"std::pmr::u8string", L"std::pmr::u16string", L"std::pmr::u32string",
            L"pmr::basic_string", L"pmr::string", L"pmr::wstring", L"pmr::u8string",
            L"pmr::u16string", L"pmr::u32string",
            // MFC, ATL
            L"CString", L"_bstr_t",
            // formatting functions that should be skipped over
            L"wxString::Format" };

        // debugging & system call functions that should never have
        // their string parameters translated
        m_internal_functions = {
            // attributes
            L"deprecated", L"nodiscard", L"_Pragma",
            // assert functions
            L"check_assertion", L"static_assert", L"assert", L"Assert",
            L"__android_log_assert",
            // wxWidgets functions and macros
            L"wxDEPRECATED_MSG", L"wxSTC_DEPRECATED_MACRO_VALUE",
            L"wxPG_DEPRECATED_MACRO_VALUE",
            L"GetExt", L"SetExt", L"XRCID", L"wxSystemOptions::GetOptionInt",
            L"WXTRACE", L"wxTrace", L"wxDATETIME_CHECK",
            L"wxASSERT", L"wxASSERT_MSG", L"wxASSERT_LEVEL_2", L"wxASSERT_LEVEL_2_MSG",
            L"wxOnAssert", L"wxCHECK", L"wxCHECK2", L"wxCHECK2_MSG",
            L"wxCHECK_MSG", L"wxCHECK_RET", L"wxCOMPILE_TIME_ASSERT",
            L"wxPROPERTY_FLAGS", L"wxPROPERTY", L"wxMISSING_IMPLEMENTATION",
            L"wxCOMPILE_TIME_ASSERT2", L"wxFAIL_MSG", L"wxFAILED_HRESULT_MSG",
            L"ExecCommand", L"CanExecCommand", L"IgnoreAppSubDir", L"put_designMode",
            L"SetExtension", L"wxSystemOptions::SetOption",
            L"wxFileName::CreateTempFileName", L"wxExecute", L"SetFailedWithLastError",
            L"wxIconHandler", L"wxBitmapHandler", L"OutputDumpLine", L"wxFileTypeInfo",
            L"TAG_HANDLER_BEGIN", L"FDEBUG", L"MDEBUG", L"wxVersionInfo",
            L"Platform::DebugPrintf", L"wxGetCommandOutput",
            // Catch2
            L"TEST_CASE", L"BENCHMARK", L"TEMPLATE_TEST_CASE", L"SECTION",
            L"DYNAMIC_SECTION", L"REQUIRE", L"REQUIRE_THROWS_WITH", L"REQUIRE_THAT",
            L"CHECK", L"CATCH_ENFORCE", L"INFO", L"SUCCEED",
            L"SCENARIO", L"GIVEN", L"AND_GIVEN", L"WHEN", L"THEN",
            L"SCENARIO_METHOD", L"WARN", L"TEST_CASE_METHOD",
            L"Catch::Clara::Arg", L"Catch::TestCaseInfo", L"GENERATE",
            L"CATCH_INTERNAL_ERROR", L"CATCH_ERROR", L"CATCH_MAKE_MSG",
            L"INTERNAL_CATCH_DYNAMIC_SECTION", L"CATCH_RUNTIME_ERROR",
            L"CATCH_INTERNAL_ERROR", L"CATCH_REQUIRE_THROWS_WIT",
            L"CATCH_SUCCEED", L"CATCH_INFO", L"CATCH_UNSCOPED_INFO",
            L"CATCH_WARN", L"CATCH_SECTION",
            // CppUnit
            L"CPPUNIT_ASSERT", L"CPPUNIT_ASSERT_EQUAL", L"CPPUNIT_ASSERT_DOUBLES_EQUAL",
            // low-level printf functions
            L"wprintf", L"printf", L"sprintf", L"snprintf", L"fprintf", L"wxSnprintf",
            // GTK
            L"gtk_tree_view_column_new_with_attributes", L"gtk_assert_dialog_append_text_column",
            L"gtk_assert_dialog_add_button_to", L"gtk_assert_dialog_add_button",
            L"g_object_set_property", L"gdk_atom_intern", L"g_object_class_override_property",
            L"g_object_get",
            // TCL
            L"Tcl_PkgRequire", L"Tcl_PkgRequireEx",
            // debugging functions from open-source projects
            L"check_assertion", L"print_debug", L"DPRINTF", L"print_warning", L"perror",
            // system functions that don't process user messages
            L"fopen", L"getenv", L"setenv", L"system", L"run", L"exec", L"execute",
            // Unix calls
            L"popen", L"dlopen", L"dlsym", L"g_signal_connect", L"g_object_set", L"handle_system_error",
            // macOS calls
            L"CFBundleCopyResourceURL",
            // Windows calls
            L"OutputDebugString", L"OutputDebugStringA", L"OutputDebugStringW",
            L"QueryValue", L"ASSERT", L"_ASSERTE", L"TRACE", L"ATLTRACE",
            L"ATLTRACE2", L"ATLENSURE", L"ATLASSERT", L"VERIFY",
            L"LoadLibrary", L"LoadLibraryEx", L"LoadModule", L"GetModuleHandle",
            L"QueryDWORDValue", L"GetTempFileName", L"QueryMultiStringValue",
            L"SetMultiStringValue", L"GetTempDirectory", L"FormatGmt", L"GetProgIDVersion",
            L"GetProfileInt", L"WriteProfileInt", L"RegOpenKeyEx",
            L"QueryStringValue", L"lpVerb", L"Invoke", L"Invoke0", 
            L"ShellExecute", L"GetProfileString", L"GetProcAddress", L"RegisterClipboardFormat",
            L"CreateIC", L"_makepath", L"_splitpath", L"VerQueryValue", L"CLSIDFromProgID",
            L"StgOpenStorage", L"InvokeN", L"CreateStream", L"DestroyElement",
            L"CreateStorage", L"OpenStream", L"CallMethod", L"PutProperty", L"GetProperty",
            // zlib
            L"Tracev", L"Trace", L"Tracevv",
            // Lua
            L"luaL_error", L"lua_pushstring", L"lua_setglobal"
            };

        m_log_functions = {
            // wxWidgets
            L"wxLogLastError", L"wxLogError", L"wxLogFatalError",
            L"wxLogGeneric", L"wxLogMessage", L"wxLogStatus", L"wxLogStatus",
            L"wxLogSysError", L"wxLogTrace", L"wxLogTrace", L"wxLogVerbose",
            L"wxLogWarning", L"wxLogDebug", L"wxLogApiError", L"LogTraceArray",
            L"DoLogRecord", L"DoLogText", L"DoLogTextAtLevel", L"LogRecord",
            L"DDELogError", L"LogTraceLargeArray",
            // SDL
            L"SDL_Log", L"SDL_LogCritical", L"SDL_LogDebug", L"SDL_LogError",
            L"SDL_LogInfo", L"SDL_LogMessage", L"SDL_LogMessageV", L"SDL_LogVerbose",
            L"SDL_LogWarn"
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
            L"foreground-set", L"background-set",
            L"weight-set", L"style-set", L"underline-set", L"size-set", L"charset",
            L"xml", L"gdiplus", L"Direct2D", L"DirectX", L"localhost",
            L"32 bit", L"32-bit", L"64 bit", L"64-bit",
            // RTF font families
            L"fnil", L"fdecor", L"froman", L"fscript", L"fswiss", L"fmodern", L"ftech",
            // common UNIX names (Windows versions are handled by more
            // complex regex expressions elsewhere)
            L"UNIX", L"macOS", L"Apple Mac OS", L"Apple Mac OS X", L"OSX",
            L"Linux", L"FreeBSD", L"POSIX", L"NetBSD" };

        m_keywords = { L"return", L"else", L"if", L"goto", L"new", L"delete",
                       L"throw" };

        // common font faces that we would usually ignore (client can add to this)
        m_font_names = { L"Arial", L"Courier New", L"Garamond", L"Calibri", L"Gabriola",
                         L".Helvetica Neue DeskInterface", L".Lucida Grande UI",
                         L"Times New Roman", L"Georgia", L"Segoe UI", L"Segoe Script",
                         L"Century Gothic", L"Century", L"Cascadia Mono", L"URW Bookman L",
                         L"AR BERKLEY", L"Brush Script", L"Consolas", L"Century Schoolbook L",
                         L"Lucida Grande", L"Helvetica Neue", L"Liberation Serif", L"Luxi Serif",
                         L"Ms Shell Dlg", L"Ms Shell Dlg 2", L"Bitstream Vera Serif", L"URW Palladio L",
                         L"URW Chancery L", L"Comic Sans MS", L"DejaVu Serif", L"DejaVu LGC Serif",
                         L"Nimbus Sans L", L"URW Gothic L", L"Lucida Sans", L"Andale Mono",
                         L"Luxi Sans", L"Liberation Sans", L"Bitstream Vera Sans", L"DejaVu LGC Sans",
                         L"DejaVu Sans", L"Nimbus Mono L", L"Lucida Sans Typewriter", L"Luxi Mono",
                         L"DejaVu Sans Mono", L"DejaVu LGC Sans Mono", L"Bitstream Vera Sans Mono",
                         L"Liberation Mono" };

        m_file_extensions = { // documents
                            L"xml", L"html", L"htm", L"xhtml", L"rtf",
                            L"doc", L"docx", L"dot", L"docm", L"txt", L"ppt", L"pptx",
                            L"pdf", L"ps", L"odt", L"ott", L"odp", L"otp", L"pptm",
                            L"md",
                            // help files
                            L"hhc", L"hhk", L"hhp",
                            // spreadsheets
                            L"xls", L"xlsx", L"ods", L"csv",
                            // image formats
                            L"gif", L"jpg", L"jpeg", L"jpe", L"bmp", L"tiff", L"tif",
                            L"png", L"tga", L"svg", L"xcf", L"ico", L"psd",
                            L"hdr", L"pcx",
                            // webpages
                            L"asp", L"aspx", L"cfm",
                            L"cfml", L"php", L"php3", L"php4",
                            L"sgml", L"wmf", L"js",
                            // style sheets
                            L"css",
                            // movies
                            L"mov", L"qt", L"rv", L"rm",
                            L"wmv", L"mpg", L"mpeg", L"mpe",
                            L"avi",
                            // music
                            L"mp3", L"wav", L"wma", L"midi",
                            L"ra", L"ram",
                            // programs
                            L"exe", L"swf", L"vbs",
                            // source files
                            L"cpp", L"h", L"c", L"idl",
                            // compressed files
                            L"gzip", L"bz2" };

        // variables whose CTORs take a string that should never be translated
        m_variable_types_to_ignore = { L"wxUxThemeHandle", L"wxRegKey", 
                            L"wxLoadedDLL", L"wxConfigPathChanger", L"wxWebViewEvent", 
                            L"wxFileSystemWatcherEvent", L"wxStdioPipe",
                            L"wxCMD_LINE_CHARS_ALLOWED_BY_SHORT_OPTION", L"vmsWarningHandler",
                            L"vmsErrorHandler", L"wxFFileOutputStream", L"wxFFile", L"wxFileName",
                            L"wxColor", L"wxColour", L"wxFont",
                            L"LOGFONTW", L"SecretSchema", L"GtkTypeInfo",
                            L"wxRegEx", L"wregex", L"std::wregex", L"regex", L"std::regex",
                            L"wxDataObjectSimple" };

        add_variable_name_pattern_to_ignore(std::wregex(L"^debug.*", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(std::wregex(L"^stacktrace.*", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(std::wregex(L"([[:alnum:]_\\-])*xpm",
                                            std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(std::wregex(L"xpm([[:alnum:]_\\-])*",
            std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(std::wregex(L"wxColourDialogNames"));
        add_variable_name_pattern_to_ignore(std::wregex(L"wxColourTable"));
        }

    //--------------------------------------------------
    void i18n_review::process_quote(wchar_t* currentTextPos, const wchar_t* quoteEnd,
        const wchar_t* functionVarNamePos,
        const std::wstring& variableName, const std::wstring& functionName,
        const std::wstring& variableType,
        const std::wstring& deprecatedMacroEncountered)
        {
        if (deprecatedMacroEncountered.length() &&
            (m_reviewStyles & check_deprecated_macros))
            {
            m_deprecated_macros.emplace_back(
                string_info(deprecatedMacroEncountered,
                    string_info::usage_info(
                        string_info::usage_info::usage_type::function,
                        std::wstring{}, std::wstring{}),
                    m_file_name,
                    get_line_and_column(currentTextPos - m_file_start)));
            }

        if (variableName.length())
            {
            process_variable(variableType, variableName,
                std::wstring(currentTextPos, quoteEnd - currentTextPos), (currentTextPos - m_file_start));
            }
        else if (functionName.length())
            {
            if (is_diagnostic_function(functionName))
                {
                m_internal_strings.emplace_back(string_info(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(
                        string_info::usage_info::usage_type::function,
                        functionName, std::wstring{}),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start)));
                }
            else if (m_localization_functions.find(functionName) !=
                m_localization_functions.cend())
                {
                m_localizable_strings.emplace_back(string_info(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(
                        string_info::usage_info::usage_type::function,
                        functionName, std::wstring{}),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start)));

                assert(functionVarNamePos);
                if (functionVarNamePos &&
                    m_reviewStyles & check_suspect_l10n_string_usage)
                    {
                    std::wstring functionNameOuter, variableNameOuter,
                                    variableTypeOuter, deprecatedMacroOuterEncountered;
                    read_var_or_function_name(functionVarNamePos, m_file_start,
                                              functionNameOuter, variableNameOuter,
                                              variableTypeOuter, deprecatedMacroOuterEncountered);
                    if (deprecatedMacroOuterEncountered.length() &&
                        (m_reviewStyles & check_deprecated_macros))
                        {
                        m_deprecated_macros.emplace_back(
                            string_info(deprecatedMacroOuterEncountered,
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::function,
                                    std::wstring{}, std::wstring{}),
                                m_file_name,
                                get_line_and_column(currentTextPos - m_file_start)));
                        }
                    // internal functions
                    if (is_diagnostic_function(functionNameOuter) ||
                        // CTORs whose arguments should not be translated
                        m_variable_types_to_ignore.find(functionNameOuter) !=
                        m_variable_types_to_ignore.cend())
                        {
                        m_localizable_strings_in_internal_call.emplace_back(
                            string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                            string_info::usage_info(
                                string_info::usage_info::usage_type::function,
                                functionNameOuter, std::wstring{}),
                            m_file_name,
                            get_line_and_column(currentTextPos - m_file_start)));
                        }
                    // untranslatable variable types
                    else if (m_variable_types_to_ignore.find(variableTypeOuter) !=
                        m_variable_types_to_ignore.cend())
                        {
                        m_localizable_strings_in_internal_call.emplace_back(
                            string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                            string_info::usage_info(
                                string_info::usage_info::usage_type::variable,
                                variableNameOuter, variableTypeOuter),
                            m_file_name,
                            get_line_and_column(currentTextPos - m_file_start)));
                        }
                    // untranslatable variable names (e.g., debugMsg)
                    else if (variableNameOuter.length())
                        {
                        try
                            {
                            for (const auto& reg : get_ignored_variable_patterns())
                                {
                                if (std::regex_match(variableNameOuter, reg))
                                    {
                                    m_localizable_strings_in_internal_call.emplace_back(
                                        string_info(std::wstring(currentTextPos, quoteEnd-currentTextPos),
                                        string_info::usage_info(
                                            string_info::usage_info::usage_type::variable,
                                            variableNameOuter, variableTypeOuter),
                                        m_file_name,
                                        get_line_and_column(currentTextPos - m_file_start)));
                                    break;
                                    }
                                }
                            }
                        catch (const std::exception& exp)
                            { log_message(variableNameOuter, lazy_string_to_wstring(exp.what()), (currentTextPos - m_file_start)); }
                        }
                    }
                }
            else if (m_non_localizable_functions.find(functionName) !=
                m_non_localizable_functions.cend())
                {
                m_marked_as_non_localizable_strings.emplace_back(
                    string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(
                        string_info::usage_info::usage_type::function,
                        functionName, std::wstring{}),
                    m_file_name,
                    get_line_and_column(currentTextPos - m_file_start)));
                }
            else if (m_variable_types_to_ignore.find(functionName) !=
                m_variable_types_to_ignore.cend())
                {
                m_internal_strings.emplace_back(
                    string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(
                        string_info::usage_info::usage_type::function,
                        functionName, std::wstring{}),
                    m_file_name,
                    get_line_and_column(currentTextPos - m_file_start)));
                }
            else if (is_keyword(functionName))
                {
                classify_non_localizable_string(
                    string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(
                        string_info::usage_info::usage_type::orphan,
                        std::wstring{}, std::wstring{}),
                    m_file_name,
                    get_line_and_column(currentTextPos - m_file_start)));
                }
            else
                {
                classify_non_localizable_string(
                    string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(
                        string_info::usage_info::usage_type::function,
                        functionName, std::wstring{}),
                    m_file_name,
                    get_line_and_column(currentTextPos - m_file_start)));
                }
            }
        else
            {
            classify_non_localizable_string(
                string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                string_info::usage_info(
                    string_info::usage_info::usage_type::orphan,
                    std::wstring{}, std::wstring{}),
                m_file_name,
                get_line_and_column(currentTextPos - m_file_start)));
            }
        clear_section(currentTextPos, quoteEnd + 1);
        }

    //--------------------------------------------------
    void i18n_review::process_variable(const std::wstring& variableType,
        const std::wstring& variableName,
        const std::wstring& value, const size_t quotePosition)
        {
    #ifndef NDEBUG
        if (variableType.length() &&
            get_ignored_variable_types().find(variableType) ==
                get_ignored_variable_types().cend() &&
            m_ctors_to_ignore.find(variableType) == m_ctors_to_ignore.cend() &&
            (variableType.length() < 5 ||
                std::wstring_view(variableType.c_str(), 5).compare(std::wstring_view{ L"std::", 5}) != 0) &&
            (variableType.length() < 2 ||
                std::wstring_view(variableType.c_str(), 2).compare(std::wstring_view{ L"wx", 2 }) != 0) &&
            (variableType.length() < 3 ||
                std::wstring_view(variableType.c_str(), 3).compare(std::wstring_view{ L"_wx", 3 }) != 0) &&
            (variableType.length() < 2 ||
                std::wstring_view(variableType.c_str(), 2).compare(std::wstring_view{ L"My", 2 }) != 0) &&
            variableType != L"Bar" &&
            variableType != L"ifstream" &&
            variableType != L"ofstream" &&
            variableType != L"const" &&
            variableType != L"char" &&
            variableType != L"int" &&
            variableType != L"UINT" &&
            variableType != L"size_t" &&
            variableType != L"float" &&
            variableType != L"double" &&
            variableType != L"static_cast" &&
            variableType != L"StyleInfo" &&
            variableType != L"Utf8CharBuffer" &&
            variableType != L"rgbRecord" &&
            variableType != L"LPCTSTR" &&
            variableType != L"CDialog" &&
            variableType != L"LanguageInfo" &&
            variableType != L"MessageParameters")
            { log_message(variableType, L"New variable type detected.", quotePosition); }
    #endif
        if (get_ignored_variable_types().find(variableType) !=
            get_ignored_variable_types().cend())
            {
            m_internal_strings.emplace_back(
                string_info(value,
                string_info::usage_info(string_info::usage_info::usage_type::variable, variableName, variableType),
                m_file_name, get_line_and_column(quotePosition)));
            return;
            }
        else if (get_ignored_variable_patterns().size())
            {
            try
                {
                bool matchedInternalVar(false);
                for (const auto& reg : get_ignored_variable_patterns())
                    {
                    if (std::regex_match(variableName, reg))
                        {
                        m_internal_strings.emplace_back(
                            string_info(value,
                            string_info::usage_info(
                                string_info::usage_info::usage_type::variable, variableName, variableType),
                            m_file_name, get_line_and_column(quotePosition)));
                        matchedInternalVar = true;
                        break;
                        }
                    }
                // didn't match any known internal variable name provided by user?
                if (!matchedInternalVar)
                    {
                    classify_non_localizable_string(
                        string_info(value,
                        string_info::usage_info(
                            string_info::usage_info::usage_type::variable, variableName, variableType),
                        m_file_name, get_line_and_column(quotePosition)));
                    }
                }
            catch (const std::exception& exp)
                {
                log_message(variableName, lazy_string_to_wstring(exp.what()), quotePosition);
                classify_non_localizable_string(
                    string_info(value,
                    string_info::usage_info(
                        string_info::usage_info::usage_type::variable, variableName, variableType),
                    m_file_name, get_line_and_column(quotePosition)));
                }
            }
        else
            {
            classify_non_localizable_string(
                string_info(value,
                string_info::usage_info(
                    string_info::usage_info::usage_type::variable, variableName, variableType),
                m_file_name, get_line_and_column(quotePosition)));
            }
        }

    //--------------------------------------------------
    void i18n_review::reserve(const size_t fileCount)
        {
        // one message per file is more than reasonable
        m_error_log.reserve(fileCount);
        constexpr size_t resourcesPerFile = 100;
        m_localizable_strings.reserve(resourcesPerFile *fileCount);
        m_not_available_for_localization_strings.reserve(resourcesPerFile *fileCount);
        m_marked_as_non_localizable_strings.reserve(resourcesPerFile *fileCount);
        m_internal_strings.reserve(resourcesPerFile *fileCount);
        m_unsafe_localizable_strings.reserve(resourcesPerFile *fileCount);
        m_deprecated_macros.reserve(resourcesPerFile *fileCount);
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
            return (std::regex_match(functionName, m_diagnostsic_function_regex) ||
                    (m_internal_functions.find(functionName) !=
                        m_internal_functions.cend()) ||
                    (!can_log_messages_be_translatable() &&
                     m_log_functions.find(functionName) !=
                        m_log_functions.cend()));
            }
        catch (const std::exception& exp)
            {
            log_message(functionName, lazy_string_to_wstring(exp.what()),
                std::wstring::npos);
            return true;
            }
        }

    //--------------------------------------------------
    bool i18n_review::is_untranslatable_string(std::wstring str,
                                               const bool limitWordCount) const
        {
        static const std::wregex oneWordRE{ LR"((\b[\w'\-]+([\.\-\/:]*[\w'\-]*)*))" };
        static const std::wregex loremIpsum(L"Lorem ipsum.*");

        i18n_string_util::replace_escaped_control_chars(str);
        string_util::trim(str);
        // see if a function signature before stripping printf commands and whatnot
        if ((std::regex_match(str, m_function_signature_regex) ||
            std::regex_match(str, m_open_function_signature_regex)) &&
            // but allow something like "Item(s)"
            !std::regex_match(str, m_plural_regex))
            { return true; }

        i18n_string_util::remove_hex_color_values(str);
        i18n_string_util::remove_printf_commands(str);
        i18n_string_util::decode_escaped_unicode_values(str);
        string_util::trim(str);
        // strip control characters (these wreak havoc with the regex parser)
        for (auto& ch : str)
            {
            if (ch == L'\n' || ch == L'\t' || ch == L'\r')
                { ch = L' '; }
            }
        string_util::trim(str);

        try
            {
            if (limitWordCount)
                {
                // see if it has enough words
                const auto matchCount
                    {
                    std::distance(
                        std::wsregex_iterator(str.cbegin(), str.cend(), oneWordRE),
                        std::wsregex_iterator())
                    };
                if (static_cast<size_t>(matchCount) <
                    get_min_words_for_classifying_unavailable_string())
                    { return true; }
                }
            // Handle HTML syntax that is hard coded in the source file.
            // Strip it down and then see if what's left contains translatable content.
            // Note that we skip any punctuation (not word characters, excluding '<')
            // in front of the initial '<' (sometimes there are braces and brackets
            // in front of the HTML tags).
            if (std::regex_match(str, m_html_regex) ||
                std::regex_match(str, m_html_element_regex) ||
                std::regex_match(str, m_html_tag_regex) ||
                std::regex_match(str, m_html_tag_unicode_regex))
                {
                str = std::regex_replace(str,
                    std::wregex(L"<[?]?[A-Za-z0-9+_/\\-\\.'\"=;:!%[:space:]\\\\,()]+[?]?>"), L"");
                // strip things like &ldquo;
                str = std::regex_replace(str, std::wregex(L"&[[:alpha:]]{2,5};"), L"");
                str = std::regex_replace(str, std::wregex(L"&#[[:digit:]]{2,4};"), L"");
                }

            // Nothing but punctuation? If that's OK to allow, then let it through.
            if (is_allowing_translating_punctuation_only_strings() &&
                std::regex_match(str,std::wregex(L"[[:punct:]]+")))
                { return false; }

            // "N/A" is OK to translate, but it won't meet the criterion of at least
            // two consecutive letters, so check for that first.
            if (str.length() == 3 &&
                string_util::is_either(str[0], L'N', L'n') &&
                str[1] == L'/' &&
                string_util::is_either(str[2], L'A', L'a'))
                { return false; }
            else if (str.length() <= 1 ||
                // not at least two letters together
                !std::regex_search(str, m_2letter_regex) ||
                // single word (no spaces or word separators) and more than 20 characters--
                // doesn't seem like a real word meant for translation
                (str.length() > 20 &&
                    str.find_first_of(L" \n\t\r/-") == std::wstring::npos &&
                    str.find(L"\\n") == std::wstring::npos &&
                    str.find(L"\\r") == std::wstring::npos &&
                    str.find(L"\\t") == std::wstring::npos) ||
                m_known_internal_strings.find(str.c_str()) != m_known_internal_strings.end())
                { return true; }
            // RTF text
            else if (str.compare(0, 3, LR"({\\)") == 0)
                { return true; }
            // social media hashtag (or formatting code of some sort)
            else if (std::regex_match(str, m_hashtag_regex))
                { return true; }
            else if (std::regex_match(str, m_key_shortcut_regex))
                { return true; }
            // if we know it had at least one word (and spaces) at this point,
            // then it being more than 200 characters means that it probably is
            // a real user-message (not an internal string)
            else if (str.length() > 200 &&
                !std::regex_match(str, loremIpsum))
                { return false; }

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
            if (is_font_name(str.c_str()) ||
                is_file_extension(str.c_str()) ||
                i18n_string_util::is_file_address(str.c_str(), str.length()))
                { return true; }
            // nothing seems to match, probably a real string
            return false;
            }
        catch (const std::exception& exp)
            {
            log_message(str, lazy_string_to_wstring(exp.what()), std::wstring::npos);
            return false;
            }
        }

    //--------------------------------------------------
    std::wstring i18n_review::collapse_multipart_string(const std::wstring& str)
        {
        // leave raw strings as-is
        if (str.length() >= 2 && str.front() == L'(' &&
            str.back() == L')')
            { return str; }
        std::wregex re(LR"(([^\\])("[\s]*"))");
        return std::regex_replace(str, re, L"$1");
        }

    //--------------------------------------------------
    void i18n_review::process_strings()
        {
        const auto processStrings = [this](auto& strings)
            {
            std::for_each(strings.begin(),
                          strings.end(),
                [this](auto& val)
                { val.m_string = collapse_multipart_string(val.m_string); });
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
            if (str.m_usage.m_value.empty() && str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                    L"Unknown function or variable assignment for this string.", std::wstring::npos);
                }
            }
        for (const auto& str : m_not_available_for_localization_strings)
            {
            if (str.m_usage.m_value.empty() && str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                    L"Unknown function or variable assignment for this string.", std::wstring::npos);
                }
            }
        for (const auto& str : m_marked_as_non_localizable_strings)
            {
            if (str.m_usage.m_value.empty() && str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                    L"Unknown function or variable assignment for this string.", std::wstring::npos);
                }
            }
        for (const auto& str : m_internal_strings)
            {
            if (str.m_usage.m_value.empty() && str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                    L"Unknown function or variable assignment for this string.", std::wstring::npos);
                }
            }
        for (const auto& str : m_unsafe_localizable_strings)
            {
            if (str.m_usage.m_value.empty() && str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                    L"Unknown function or variable assignment for this string.", std::wstring::npos);
                }
            }
        }

    //--------------------------------------------------
    const wchar_t* i18n_review::read_var_or_function_name(const wchar_t* startPos,
            const wchar_t* const startSentinel,
            std::wstring& functionName, std::wstring& variableName, std::wstring& variableType,
            std::wstring& deprecatedMacroEncountered)
        {
        functionName.clear();
        variableName.clear();
        variableType.clear();
        deprecatedMacroEncountered.clear();
        long closeParenCount{ 0 };
        long closeBraseCount{ 0 };
        bool quoteWrappedInCTOR{ false };
        const wchar_t* functionOrVarNamePos = startPos;

        /// @todo experimental!!! Reads the variable type from a variable constructed from a string.
        const auto readVarType = [&]()
            {
            variableType.clear();
            if (functionOrVarNamePos == startSentinel)
                { return; }
            --functionOrVarNamePos;
            while (functionOrVarNamePos > startSentinel &&
                std::iswspace(*functionOrVarNamePos))
                { --functionOrVarNamePos; }
            auto typeEnd = functionOrVarNamePos+1;
            // if a template, then step over (going backwards) the template arguments
            // to get to the root type
            if (typeEnd - 1 > startSentinel &&
                typeEnd[-1] == L'>')
                {
                // if a pointer accessor (->) then bail as it won't be a variable assignment
                if (typeEnd - 2 > startSentinel &&
                    typeEnd[-2] == L'-')
                    { return; }
                assert(functionOrVarNamePos >= startSentinel);
                const auto openingAngle =
                    string_util::find_last_of(startSentinel, L'<', functionOrVarNamePos - startSentinel);
                if (openingAngle == std::wstring::npos)
                    {
                    log_message(L"Template parse error", L"Unable to find opening < for template variable.",
                                functionOrVarNamePos - startSentinel);
                    return;
                    }
                functionOrVarNamePos = startSentinel + openingAngle;
                }
            while (functionOrVarNamePos > startSentinel &&
                is_valid_name_char_ex(*functionOrVarNamePos))
                { --functionOrVarNamePos; }
            if (!is_valid_name_char_ex(*functionOrVarNamePos))
                { ++functionOrVarNamePos; }
            variableType.assign(functionOrVarNamePos, typeEnd - functionOrVarNamePos);
            remove_decorations(variableType);

            // ignore case labels
            if (is_keyword(variableType) ||
                (variableType.length() && variableType.back() == L':'))
                { variableType.clear(); }
            };

        while (startPos > startSentinel)
            {
            if (*startPos == L')')
                {
                ++closeParenCount;
                --startPos;
                }
            else if (*startPos == L'}')
                {
                ++closeBraseCount;
                --startPos;
                }
            else if (*startPos == L'(' ||
                *startPos == L'{')
                {
                const auto currentOpeningChar{ *startPos };
                --startPos;
                // if just closing the terminating parenthesis for a function
                // call in the list of parameters, then skip it and keep going
                // to find the outer function call that this string really belongs to.
                if (currentOpeningChar == L'(')
                    { --closeParenCount; }
                else if (currentOpeningChar == L'{')
                    { --closeBraseCount; }
                if (closeParenCount >= 0 &&
                    closeBraseCount >= 0)
                    { continue; }
                // skip whitespace between open parenthesis and function name
                while (startPos > startSentinel &&
                       std::iswspace(*startPos))
                    { --startPos; }
                functionOrVarNamePos = startPos;
                while (functionOrVarNamePos > startSentinel &&
                    is_valid_name_char_ex(*functionOrVarNamePos) )
                    { --functionOrVarNamePos; }
                // If we are on the start of the text, then see if we need to
                // include that character too. We may have short circuited because
                // we reached the start of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos) )
                    { ++functionOrVarNamePos; }
                functionName.assign(functionOrVarNamePos, (startPos+1)-functionOrVarNamePos);
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
                        { closeParenCount = 0; }
                    else if (currentOpeningChar == L'{')
                        { closeBraseCount = 0; }
                    if (m_deprecated_string_macros.find(functionName) !=
                        m_deprecated_string_macros.cend())
                        { deprecatedMacroEncountered = functionName; }
                    functionName.clear();
                    // now we should be looking for a + operator, comma, or ( or { proceeding this
                    // (unless we are already on it because we stepped back too far
                    //  due to the string being inside of an empty parenthesis)
                    if (*startPos != L',' &&
                        *startPos != L'+'&&
                        *startPos != L'&'&&
                        *startPos != L'=')
                        { quoteWrappedInCTOR = true; }
                    if (!hasExtraneousParens)
                        { --startPos; }
                    continue;
                    }
                // construction of a variable type that takes
                // non-localizable strings, just skip it entirely
                else if (m_variable_types_to_ignore.find(functionName) !=
                    m_variable_types_to_ignore.cend())
                    { break; }
                // stop if a legit function call in front of parenthesis
                if (functionName.length())
                    {
                    if (variableName.empty() &&
                        functionOrVarNamePos >= startSentinel && !is_keyword(functionName))
                        {
                        readVarType();

                        if (variableType.length())
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
            //  is a parameter to a function.)
            else if (*startPos == L'=' &&
                     startPos[1] != L'=' &&
                     startPos > startSentinel &&
                     *(startPos-1) != L'=' &&
                     *(startPos-1) != L'!' &&
                     *(startPos-1) != L'>' &&
                     *(startPos-1) != L'<')
                {
                --startPos;
                // skip spaces (and "+=" tokens)
                while (startPos > startSentinel &&
                       (std::iswspace(*startPos) || *startPos == L'+'))
                    { --startPos; }
                // skip array info
                if (startPos > startSentinel &&
                    *startPos == L']')
                    {
                    while (startPos > startSentinel &&
                            *startPos != L'[')
                        { --startPos; }
                    --startPos;
                    while (startPos > startSentinel &&
                        std::iswspace(*startPos))
                        { --startPos; }
                    }
                functionOrVarNamePos = startPos;
                while (functionOrVarNamePos > startSentinel &&
                    is_valid_name_char_ex(*functionOrVarNamePos) )
                    { --functionOrVarNamePos; }
                // If we are on the start of the text, then see if we need to include that
                // character too. We may have short circuited because we reached the start
                // of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos) )
                    { ++functionOrVarNamePos; }
                variableName.assign(functionOrVarNamePos, (startPos+1) - functionOrVarNamePos);

                readVarType();

                if (variableName.length())
                    { break; }
                }
            else if (std::iswspace(*startPos))
                { --startPos; }
            else if (quoteWrappedInCTOR &&
                     (*startPos == L',' ||
                      *startPos == L'+' ||
                      *startPos == L'&'))
                { quoteWrappedInCTOR = false; }
            else if (quoteWrappedInCTOR &&
                     *startPos != L',' &&
                     *startPos != L'+' &&
                     *startPos != L'&')
                { break; }
            else
                { --startPos; }
            }

        return functionOrVarNamePos;
        }

    //--------------------------------------------------
    std::pair<size_t, size_t> i18n_review::get_line_and_column(size_t position) const noexcept
        {
        if (position == std::wstring::npos)
            { return std::make_pair(std::wstring::npos, std::wstring::npos); }

        auto startSentinel = m_file_start;
        if (!startSentinel)
            { return std::make_pair(std::wstring::npos, std::wstring::npos); }
        size_t nextLinePosition{ 0 }, lineCount{ 0 };
        while ((nextLinePosition = std::wcscspn(startSentinel, L"\r\n")) < position)
            {
            ++lineCount;
            if (nextLinePosition+1 < position && startSentinel[nextLinePosition] == L'\r' &&
                startSentinel[nextLinePosition+1] == L'\n')
                {
                startSentinel += nextLinePosition+2;
                position -= nextLinePosition+2;
                }
            else
                {
                startSentinel += nextLinePosition+1;
                position -= nextLinePosition+1;
                }
            }
        // make one-indexed
        return std::make_pair(lineCount+1, position+1);
        }
    }
