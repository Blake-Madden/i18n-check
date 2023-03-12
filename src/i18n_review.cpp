///////////////////////////////////////////////////////////////////////////////
// Name:        i18n_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "i18n_review.h"

namespace i18n_check
    {
    //--------------------------------------------------
    i18n_review::i18n_review() :
        // HTML, but also includes some GTK formatting tags
        m_html_regex(L"[^[:alnum:]<]*<(span|object|property|div|p|ul|ol|li|img|html|[?]xml|meta|body|table|tbody|tr|td|thead|head|title|a[[:space:]]|!--|/|!DOCTYPE|br|center|dd|em|dl|dt|tt|font|form|h[[:digit:]]|hr|main|map|pre|script).*", std::regex_constants::icase),
        m_html_tag_regex(L"&[[:alpha:]]{2,5};.*"),
        m_html_tag_unicode_regex(L"&#[[:digit:]]{2,4};.*"),
        // <doc-val>Some text</doc-val>
        m_html_element_regex(L"<[a-zA-Z0-9_\\-]+>[[:print:][:cntrl:]]*</[a-zA-Z0-9_\\-]+>", std::regex_constants::icase),
        m_2letter_regex(L"[[:alpha:]]{2,}"),
        m_hashtag_regex(L"#[[:alnum:]]{2,}"),
        m_function_signature_regex(L"[[:alnum:]]{2,}[(][[:alnum:]]+(,[[:space:]]*[[:alnum:]]+)*[)]"),
        m_plural_regex(L"[[:alnum:]]{2,}[(]s[)]"),
        m_open_function_signature_regex(L"[[:alnum:]]{2,}[(]"),
        m_key_shortcut_regex(L"(CTRL|SHIFT|CMD|ALT)([+](CTRL|SHIFT|CMD|ALT))*([+][[:alnum:]])+", std::regex_constants::icase),
        m_assert_regex(L"([a-zA-Z0-9_]*|^)ASSERT([a-zA-Z0-9_]*|$)"),
        m_allow_translating_punctuation_only_strings(false)
        {
        m_untranslatable_regexes = {
            // nothing but numbers, punctuation, control characters?
            std::wregex(L"([[:digit:][:space:][:punct:][:cntrl:]]|\\\\[rnt])+"),
            // HTML entities
            std::wregex(L"&[#]?[xX]?[[:alnum:]]+;"),
            // An opening HTML element
            std::wregex(L"<(body|html|img|head|meta|style|span|p|tr|td)"),
            // XML elements
            std::wregex(L"<[A-Za-z0-9_/\\-.'\"=;:[:space:]]+>"),
            std::wregex(L"<[A-Za-z0-9_/\\-.'\"=;:[:space:]]+>[[:space:][:digit:][:punct:]]*<[A-Za-z0-9_/\\-.']*>"),
            std::wregex(L"<([A-Za-z0-9_\\-.]+[[:space:]]*){1,2}=[[:punct:]A-Za-z0-9]*"),
            // <image x=%d y=\"%d\" width = '%dpx' height="%dpx"
            std::wregex(L"<[A-Za-z0-9_\\-.]+[[:space:]]*([A-Za-z0-9_\\-.]+[[:space:]]*=[[:space:]]*[\"'\\\\]{0,2}[a-zA-Z0-9\\-]*[\"'\\\\]{0,2}[[:space:]]*)+"),
            std::wregex(L"charset[[:space:]]*=.*", std::regex_constants::icase),
            // all 'X'es and spaces, usually a placeholder of some sort
            std::wregex(L"[xX ]+"),
            // Pascal-case words (e.g., "GetValueFromUser"); surrounding punctuation is stripped first.
            std::wregex(L"[[:punct:]]*[A-Z]+[a-z0-9]+([A-Z]+[a-z0-9]+)+[[:punct:]]*"),
            // camel-case words (e.g., "getValueFromUser"); surrounding punctuation is stripped first.
            std::wregex(L"[[:punct:]]*[a-z]+[[:digit:]]*([A-Z]+[a-z0-9]+)+[[:punct:]]*"),
            // formulas (e.g., ComputeNumbers() )
            std::wregex(L"[A-Za-z0-9_]{3,}[(][)]"),
            // equal sign followed by a single word is probably some sort of config file tag or formula.
            std::wregex(L"=[A-Za-z0-9_]+"),
            // character encodings
            std::wregex(L"(utf[-]?[[:digit:]]+|Shift[-_]JIS|us-ascii|windows-[[:digit:]]{4}|KOI8-R|Big5|GB2312|iso-[[:digit:]]{4}-[[:digit:]]+)", std::regex_constants::icase),
            // wwxWidgets constants
            std::wregex(L"(wx|WX)[A-Z_0-9]{2,}"),
            // ODCTask --surrounding punctuation is stripped first
            std::wregex(L"[[:punct:]]*[A-Z]{3,}[a-z_0-9]{2,}[[:punct:]]*"),
            // Config_File, "user_level_permission", "__HIGH_SCORE__"
            std::wregex(L"[_]*[a-zA-Z0-9]+(_[a-zA-Z0-9]+)+[_]*"),
            // CSS strings
            std::wregex(L"font-(style|weight|family|size|face-name|underline|point-size)[[:space:]]*[:]?.*", std::regex_constants::icase),
            std::wregex(L"text-decoration[[:space:]]*[:]?.*", std::regex_constants::icase),
            std::wregex(L"(background-)?color[[:space:]]*:.*", std::regex_constants::icase),
            std::wregex(L"style[[:space:]]*=[\"']?.*", std::regex_constants::icase),
            // local file paths & file names
            std::wregex(L"(WINDIR|Win32|System32|Kernel32|/etc|/tmp)", std::regex_constants::icase),
            std::wregex(L"(so|dll|exe|dylib|jpg|bmp|png|gif|txt|doc)", std::regex_constants::icase), // common file extension that might be missing the period
            std::wregex(L"[.][a-zA-Z0-9]{1,5}"), // file extension
            std::wregex(L"[.]DS_Store"), // macOS file
            std::wregex(L"[\\\\/]?[[:alnum:]_~!@#$%&;',+={}().^\\[\\]\\-]+([.][a-zA-Z0-9]{1,4})+"), // file name (supports multiple extensions)
            std::wregex(L"([[:alnum:]_-]+[\\\\/]){1,2}[[:alnum:]_-]+([.][a-zA-Z0-9]{1,4})+"), // ultra simple relative file path (e.g., "shaders/player1.vert")
            std::wregex(L"\\*[.][a-zA-Z0-9]{1,5}"), // wild card file extension
            std::wregex(L"([/]{1,2}[[:alnum:]_~!@#$%&;',+={}().^\\[\\]\\-]+){2,}/?"), // UNIX or web folder (needs at least 1 folder in path)
            std::wregex(L"[a-zA-Z][:]([\\\\]{1,2}[[:alnum:]_~!@#$%&;',+={}().^\\[\\]\\-]*)+"), // Windows folder
            // Windows HTML clipboard data
            std::wregex(L"(End|Start)(HTML|Fragment)[:]?[[:digit:]]*"),
            // printer commands (e.g., @PAGECOUNT@)
            std::wregex(L"@[A-Z0-9]+@"),
            // [CMD]
            std::wregex(L"\\[[A-Z0-9]+\\]"),
            // Windows OS names
            std::wregex(L"Windows (95|98|NT|ME|2000|Server|Vista|Longhorn|XP|[[:digit:]]{1,2}[.]?[[:digit:]]{0,2})[[:space:]]*[[:digit:]]{0,4}[[:space:]]*(R|SP)?[[:digit:]]{0,2}")
            };

        // functions/macros that indicate that a string will be localizable via GETTEXT (or similar mechanism)
        m_localization_functions = { L"_", L"N_", L"gettext_noop", L"gettext",
            // wxWidgets GETTEXT wrapper functions
            L"wxPLURAL", L"wxTRANSLATE", L"wxGetTranslation" };

        // functions that indicate that a string is explicitly marked to not be translatable
        m_non_localizable_functions = { L"_DT", L"DONTTRANSLATE" };

        // Constructors and macros that should be ignored
        // (when backtracing, these are skipped over, and the parser moves to the function/variable
        //  assignment to the left of these).
        m_ctors_to_ignore = {
            // Win32 text macros that should be skipped over
            L"_T", L"TEXT", L"_TEXT", L"_WIDE",L"CFSTR",
            // wxWidgets
            L"wxT", L"wxT_2", L"wxS", L"wxString", L"wxBasicString", L"wxCFStringRef",
            // standard string objects
            L"string", L"wstring", L"basic_string", L"std::string", L"std::wstring", L"std::basic_string",
            // MFC, ATL
            L"CString", L"_bstr_t",
            // formatting functions that should be skipped over
            L"wxString::Format" };

        // debugging/logging/system call functions that should never have their string parameters translated
        m_internal_functions = {
            // Diagnostic/system functions
            L"deprecated", L"_Pragma",
            // assert functions
            L"check_assertion", L"static_assert", L"assert",
            // wxWidgets functions
            L"GetExt", L"SetExt", L"XRCID", L"wxSystemOptions::GetOptionInt",
            L"wxLogLastError", L"WXTRACE", L"wxLogError", L"wxLogFatalError",
            L"wxLogGeneric", L"wxLogMessage", L"wxLogStatus", L"wxLogStatus",
            L"wxLogSysError", L"wxLogTrace", L"wxLogTrace", L"wxLogVerbose",
            L"wxLogWarning", L"wxTrace", L"wxLogDebug", L"wxLogApiError",
            L"DoLogRecord", L"DoLogText", L"DoLogTextAtLevel", L"LogRecord",
            L"wxASSERT", L"wxASSERT_MSG", L"wxASSERT_LEVEL_2", L"wxASSERT_LEVEL_2_MSG",
            L"wxOnAssert", L"wxCHECK", L"wxCHECK2", L"wxCHECK2_MSG",
            L"wxCHECK_MSG", L"wxCHECK_RET", L"wxCOMPILE_TIME_ASSERT",
            L"wxCOMPILE_TIME_ASSERT2", L"wxFAIL_MSG", L"wxFAILED_HRESULT_MSG",
            L"ExecCommand", L"CanExecCommand", L"IgnoreAppSubDir", L"put_designMode",
            L"SetExtension", L"LogTraceArray", L"wxSystemOptions::SetOption",
            L"wxFileName::CreateTempFileName",
            // GTK
            L"gtk_tree_view_column_new_with_attributes", L"gtk_assert_dialog_append_text_column",
            L"gtk_assert_dialog_add_button_to", L"gtk_assert_dialog_add_button",
            L"g_object_set_property", L"gdk_atom_intern", L"g_object_class_override_property",
            L"g_object_get",
            // TCL
            L"Tcl_PkgRequire", L"Tcl_PkgRequireEx",
            // SDL
            L"SDL_Log", L"SDL_LogCritical", L"SDL_LogDebug", L"SDL_LogError",
            L"SDL_LogInfo", L"SDL_LogMessage", L"SDL_LogMessageV", L"SDL_LogVerbose",
            L"SDL_LogWarn",
            // debugging functions from open-source projects
            L"check_assertion", L"print_debug", L"DPRINTF", L"print_warning", L"perror",
            // system functions that don't process user messages
            L"fopen", L"getenv", L"setenv", L"system",
            // Unix calls
            L"dlopen", L"dlsym", L"g_signal_connect", L"g_object_set", L"handle_system_error",
            // macOS calls
            L"CFBundleCopyResourceURL",
            // Windows calls
            L"OutputDebugString", L"OutputDebugStringA", L"OutputDebugStringW",
            L"QueryValue", L"ASSERT", L"_ASSERTE", L"TRACE", L"ATLTRACE",
            L"ATLTRACE2", L"ATLENSURE", L"ATLASSERT", L"AfxThrowOleDispatchException",
            L"LoadLibrary", L"LoadLibraryEx", L"LoadModule", L"GetModuleHandle",
            L"QueryDWORDValue", L"GetTempFileName", L"QueryMultiStringValue",
            L"SetMultiStringValue", L"GetTempDirectory", L"FormatGmt", L"GetProgIDVersion",
            L"GetProfileInt", L"WriteProfileInt", L"RegOpenKeyEx",
            L"QueryStringValue", L"lpVerb", L"Invoke", L"Invoke0", 
            L"ShellExecute", L"GetProfileString", L"GetProcAddress", L"RegisterClipboardFormat",
            L"CreateIC", L"_makepath", L"_splitpath", L"VerQueryValue", L"CLSIDFromProgID",
            L"StgOpenStorage", L"InvokeN", L"CreateStream", L"DestroyElement",
            L"CreateStorage", L"OpenStream", L"CallMethod", L"PutProperty", L"GetProperty",
            // Lua
            L"lua_setglobal"
            };

        // known strings to ignore
        m_known_internal_strings = { L"size-points", L"background-gdk", L"foreground-gdk",
            L"foreground-set", L"background-set",
            L"weight-set", L"style-set", L"underline-set", L"size-set", L"charset",
            L"xml", L"gdiplus", L"Direct2D", L"DirectX", L"localhost",
            // RTF font families
            L"fnil", L"fdecor", L"froman", L"fscript", L"fswiss", L"fmodern", L"ftech",
            // common UNIX names (Windows versions are handled by more complex regex expressions elsewhere)
            L"UNIX", L"macOS", L"OSX", L"Linux", L"FreeBSD", L"POSIX", L"NetBSD" };

        m_keywords = { L"return", L"else", L"if", L"goto", L"new", L"delete",
                       L"throw" };

        // common font faces that we would usually ignore (client can add to this)
        m_font_names = { L"Arial", L"Courier New", L"Garamond", L"Calibri", L"Gabriola",
                        L".Helvetica Neue DeskInterface", L".Lucida Grande UI",
                        L"Times New Roman", L"Georgia", L"Segoe UI", L"Segoe Script",
                        L"Century Gothic", L"Century",
                        L"AR BERKLEY", L"Brush Script",
                        L"Lucida Grande", L"Helvetica Neue" };

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
                             //style sheets
                             L"css",
                             //movies
                             L"mov", L"qt", L"rv", L"rm",
                             L"wmv", L"mpg", L"mpeg", L"mpe",
                             L"avi",
                             //music
                             L"mp3", L"wav", L"wma", L"midi",
                             L"ra", L"ram",
                             //programs
                             L"exe", L"swf", L"vbs",
                             // source files
                             L"cpp", L"h", L"c", L"idl",
                             //compressed files
                             L"gzip", L"bz2" };

        // variables whose CTORs take a string that should never be translated
        m_untranslatable_variable_types = { L"wxUxThemeHandle", L"wxRegKey", 
                                    L"wxLoadedDLL", L"wxConfigPathChanger", L"wxWebViewEvent", 
                                    L"wxFileSystemWatcherEvent", L"wxStdioPipe",
                                    L"wxCMD_LINE_CHARS_ALLOWED_BY_SHORT_OPTION", L"vmsWarningHandler",
                                    L"vmsErrorHandler",
                                    L"wxColor", L"wxColour",
                                    L"wxRegEx", L"wregex", L"std::wregex", L"regex", L"std::regex",
                                    L"wxDataObjectSimple" };

        add_variable_pattern_to_ignore(std::wregex(L"^debug.*", std::regex_constants::icase));
        }

    //--------------------------------------------------
    void i18n_review::process_variable(const std::wstring& variableType, const std::wstring& variableName,
        const std::wstring& value, const size_t quotePosition)
        {
    #ifndef NDEBUG
        if (variableType.length() &&
            get_untranslatable_variable_types().find(variableType) == get_untranslatable_variable_types().cend() &&
            variableType != L"wxString" &&
            variableType != L"wxDialog" &&
            variableType != L"wxTextInputStream" &&
            variableType != L"wxCFStringRef" &&
            variableType != L"wxStringTokenizer" &&
            variableType != L"MessageParameters")
            { log_message(variableType, "New variable type detected."); }
    #endif
        if (get_untranslatable_variable_types().find(variableType) != get_untranslatable_variable_types().cend())
            {
            m_internal_strings.emplace_back(string_info(value,
                            string_info::usage_info(string_info::usage_info::usage_type::variable, variableName),
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
                        m_internal_strings.emplace_back(string_info(value,
                            string_info::usage_info(string_info::usage_info::usage_type::variable, variableName),
                            m_file_name, get_line_and_column(quotePosition)));
                        matchedInternalVar = true;
                        break;
                        }
                    }
                //didn't match any known internal variable name provided by user?
                if (!matchedInternalVar)
                    { classify_non_localizable_string(string_info(value,
                        string_info::usage_info(string_info::usage_info::usage_type::variable, variableName),
                        m_file_name, get_line_and_column(quotePosition))); }
                }
            catch (const std::exception& exp)
                {
                log_message(variableName, exp.what());
                classify_non_localizable_string(string_info(value,
                    string_info::usage_info(string_info::usage_info::usage_type::variable, variableName),
                    m_file_name, get_line_and_column(quotePosition)));
                }
            }
        else
            {
            classify_non_localizable_string(string_info(value,
                string_info::usage_info(string_info::usage_info::usage_type::variable, variableName),
                m_file_name, get_line_and_column(quotePosition)));
            }
        }

    //--------------------------------------------------
    void i18n_review::reserve(const size_t fileCount)
        {
        m_error_log.reserve(fileCount);//one message per file is more than reasonable
        constexpr size_t resourcesPerFile = 100;
        m_localizable_strings.reserve(resourcesPerFile *fileCount);
        m_not_available_for_localization_strings.reserve(resourcesPerFile *fileCount);
        m_marked_as_non_localizable_strings.reserve(resourcesPerFile *fileCount);
        m_internal_strings.reserve(resourcesPerFile *fileCount);
        m_unsafe_localizable_strings.reserve(resourcesPerFile *fileCount);
        }

    //--------------------------------------------------
    void i18n_review::clear_results() noexcept
        {
        m_localizable_strings.clear();
        m_not_available_for_localization_strings.clear();
        m_marked_as_non_localizable_strings.clear();
        m_internal_strings.clear();
        m_unsafe_localizable_strings.clear();
        }

    //--------------------------------------------------
    bool i18n_review::is_diagnostic_function(const std::wstring& functionName) const
        {
        try
            {
            return (std::regex_match(functionName, m_assert_regex) ||
                                m_internal_functions.find(functionName) != m_internal_functions.end());
            }
        catch (const std::exception& exp)
            {
            log_message(functionName, exp.what());
            return true;
            }
        }

    //--------------------------------------------------
    bool i18n_review::is_untranslatable_string(std::wstring str) const
        {
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
            // Handle HTML syntax that is hard coded in the source file.
            // Strip it down and then see if what's left contains translatable content.
            // Note that we skip any punctuation (not word characters, excluding '<') in front of the initial '<'
            // (sometimes there are braces and brackets in front of the HTML tags).
            if (std::regex_match(str,m_html_regex) ||
                std::regex_match(str,m_html_element_regex) ||
                std::regex_match(str,m_html_tag_regex) ||
                std::regex_match(str,m_html_tag_unicode_regex))
                {
                str = std::regex_replace(str, std::wregex(L"<[?]?[A-Za-z0-9+_/\\-.'\"=;:!%[:space:]\\\\,()]+[?]?>"), L"");
                str = std::regex_replace(str, std::wregex(L"&[[:alpha:]]{2,4};"), L"");
                str = std::regex_replace(str, std::wregex(L"&#[[:digit:]]{2,4};"), L"");
                }

            // Nothing but punctuation? If that's OK to allow, then let it through.
            if (is_allowing_translating_punctuation_only_strings() &&
                std::regex_match(str,std::wregex(L"[[:punct:]]+")))
                { return false; }

            // "N/A" is OK to translate, but it won't meet the criterion of at least two consecutive letters,
            // so check for that first.
            if (str.length() == 3 &&
                string_util::is_either(str[0], L'N', L'n') &&
                str[1] == L'/' &&
                string_util::is_either(str[2], L'A', L'a'))
                { return false; }
            else if (str.length() <= 1 ||
                // not at least two letters together
                !std::regex_search(str, m_2letter_regex) ||
                // single word (no spaces or word separators) and more than 20 characters--doesn't seem like a real word meant for translation
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
            // social media hashtag (or formattint code of some sort)
            else if (std::regex_match(str, m_hashtag_regex))
                { return true; }
            else if (std::regex_match(str, m_key_shortcut_regex))
                { return true; }
            // if we know it had at least one word (and spaces) at this point, then it being more than 200
            // characters means that it probably is a real user-message (not an internal string)
            else if (str.length() > 200)
                { return false; }

            for (const auto& reg : m_untranslatable_regexes)
                {
                if (std::regex_match(str,reg))
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
            log_message(str, exp.what());
            return false;
            }
        }

    //--------------------------------------------------
    void i18n_review::run_diagnostics()
        {
        for (const auto& str : m_localizable_strings)
            {
            if (str.m_usage.m_value.empty())
                { log_message(str.m_string, "Unknown function or variable assignment for this string."); }
            }
        for (const auto& str : m_not_available_for_localization_strings)
            {
            if (str.m_usage.m_value.empty())
                { log_message(str.m_string, "Unknown function or variable assignment for this string."); }
            }
        for (const auto& str : m_marked_as_non_localizable_strings)
            {
            if (str.m_usage.m_value.empty())
                { log_message(str.m_string, "Unknown function or variable assignment for this string."); }
            }
        for (const auto& str : m_internal_strings)
            {
            if (str.m_usage.m_value.empty())
                { log_message(str.m_string, "Unknown function or variable assignment for this string."); }
            }
        for (const auto& str : m_unsafe_localizable_strings)
            {
            if (str.m_usage.m_value.empty())
                { log_message(str.m_string, "Unknown function or variable assignment for this string."); }
            }
        }

    //--------------------------------------------------
    const wchar_t* i18n_review::read_var_or_function_name(const wchar_t* startPos, const wchar_t* const startSentinel,
            std::wstring& functionName, std::wstring& variableName, std::wstring& variableType)
        {
        functionName.clear();
        variableName.clear();
        variableType.clear();
        long closeParenCount = 0;
        const wchar_t* functionOrVarNamePos = startPos;

        while (startPos > startSentinel)
            {
            if (*startPos == L')')
                {
                ++closeParenCount;
                --startPos;
                }
            else if (*startPos == L'(')
                {
                --startPos;
                // if just closing the terminating parenthesis for a function call in the list of parameters,
                // then skip it and keep going to find the outer function call that this string
                // really belongs to.
                --closeParenCount;
                if (closeParenCount >= 0)
                    { continue; }
                // skip whitespace between open parenthesis and function name
                while (startPos > startSentinel &&
                        std::iswspace(*startPos))
                    { --startPos; }
                functionOrVarNamePos = startPos;
                while (functionOrVarNamePos > startSentinel &&
                    is_valid_name_char_ex(*functionOrVarNamePos) )
                    { --functionOrVarNamePos; }
                // If we are on the start of the text, then see if we need to include that
                // character too. We may have short circuited because we reached the start of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos) )
                    { ++functionOrVarNamePos; }
                functionName.assign(functionOrVarNamePos, (startPos+1)-functionOrVarNamePos);
                remove_decorations(functionName);
                // if wrapped in a string CTOR (e.g., std::wstring), then skip it
                // and keep going backwards.
                // Or, if no function name probably means extraneous parentheses, so keep going.
                if (functionName.empty() ||
                    m_ctors_to_ignore.find(functionName) != m_ctors_to_ignore.cend())
                    {
                    startPos = std::min(startPos,functionOrVarNamePos);
                    closeParenCount = 0; //reset, the current open paren isn't relevant
                    continue;
                    }
                // construction of a variable type that takes non-localizable strings, just skip it entirely
                else if (m_untranslatable_variable_types.find(functionName) != m_untranslatable_variable_types.cend())
                    { break; }
                // stop if a legit function call in front of parenthesis
                if (functionName.length())
                    {
                    ///@todo experimental!!! Reads the variable type from a variable constructed from a string.
                    if (variableName.empty() &&
                        functionOrVarNamePos >= startSentinel && !is_keyword(functionName))
                        {
                        --functionOrVarNamePos;
                        while (functionOrVarNamePos > startSentinel &&
                            std::iswspace(*functionOrVarNamePos))
                            { --functionOrVarNamePos; }
                        auto typeEnd = functionOrVarNamePos;
                        while (functionOrVarNamePos > startSentinel &&
                            is_valid_name_char(*functionOrVarNamePos))
                            { --functionOrVarNamePos; }
                        if (!is_valid_name_char(*functionOrVarNamePos))
                            { ++functionOrVarNamePos; }
                        variableType.assign(functionOrVarNamePos, (typeEnd+1)-functionOrVarNamePos);
                        remove_decorations(variableType);

                        if (variableType.length() && !is_keyword(variableType))
                            {
                            variableName = functionName;
                            functionName.clear();
                            }
                        }
                    // end experimental
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
                // character too. We may have short circuited because we reached the start of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos) )
                    { ++functionOrVarNamePos; }
                variableName.assign(functionOrVarNamePos, (startPos+1)-functionOrVarNamePos);

                // get the type
                --functionOrVarNamePos;
                while (functionOrVarNamePos > startSentinel &&
                    std::iswspace(*functionOrVarNamePos))
                    { --functionOrVarNamePos; }
                auto typeEnd = functionOrVarNamePos+1;
                while (functionOrVarNamePos > startSentinel &&
                    is_valid_name_char_ex(*functionOrVarNamePos) )
                    { --functionOrVarNamePos; }
                variableType.assign(functionOrVarNamePos+1, typeEnd-(functionOrVarNamePos+1));
                // ignore labels
                if (variableType.length() && variableType.back() == L':')
                    { variableType.clear(); }
                if (variableName.length())
                    { break; }
                }
            else
                { --startPos; }
            }

        return functionOrVarNamePos;
        }

    //--------------------------------------------------
    std::pair<size_t, size_t> i18n_review::get_line_and_column(size_t position) noexcept
        {
        auto startSentinel = m_file_start;
        if (!startSentinel)
            { return std::make_pair(-1,-1); }
        size_t nextLinePosition{ 0 }, lastLinePosition{ 0 }, lineCount{ 0 };
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

            lastLinePosition = nextLinePosition;
            }
        // make one-indexed
        return std::make_pair(lineCount+1, position+1);
        }
    }
