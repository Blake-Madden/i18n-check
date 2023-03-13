///////////////////////////////////////////////////////////////////////////////
// Name:        cpp_i18n_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2023 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "cpp_i18n_review.h"

namespace i18n_check
    {
    //--------------------------------------------------
    void cpp_i18n_review::operator()(const wchar_t* src_text, const size_t text_length,
        const std::wstring& file_name)
        {
        if (src_text == nullptr || text_length == 0)
            { return; }
        assert(std::wcslen(src_text) == text_length);
        
        auto cppBuffer = std::make_unique<wchar_t[]>(text_length + 1);
        wchar_t* cpp_text = cppBuffer.get();
        std::wcsncpy(cpp_text, src_text, text_length);

        m_file_start = cpp_text;
        const wchar_t* const endSentinel = cpp_text + text_length;

        m_file_name = file_name;

        while (cpp_text && cpp_text + 2 < endSentinel && cpp_text[0] != 0)
            {
            // if a possible comment, then scan past it
            if (cpp_text[0] == L'/')
                {
                // see if a block comment (/*comment*/)
                if (cpp_text[1] == L'*')
                    {
                    wchar_t* end = std::wcsstr(cpp_text, L"*/");
                    if (end && end < endSentinel)
                        {
                        clear_section(cpp_text,end+2);
                        cpp_text = end + 2;
                        }
                    // can't find ending tag, so just read in the rest of the text
                    else
                        { return; }
                    }
                // or a single line comment
                else if (cpp_text[1] == L'/')
                    {
                    const size_t end = std::wcscspn(cpp_text, L"\n\r");
                    clear_section(cpp_text,cpp_text+end);
                    cpp_text += end;
                    }
                // not a comment
                else
                    { ++cpp_text; }
                }
            else if (cpp_text[0] == L'#')
                {
                cpp_text = process_preprocessor_directive(cpp_text, cpp_text-m_file_start);
                }
            else if (((cpp_text == m_file_start) ||
                !is_valid_name_char(cpp_text[-1])) &&
                is_assembly_block(cpp_text))
                {
                cpp_text = process_assembly_block(cpp_text);
                }
            // ...or an actual quote
            else if (cpp_text[0] == L'\"')
                {
                // skip escaped quotes
                if (cpp_text > m_file_start + 1 &&
                    *(cpp_text - 1) == L'\\' &&
                    *(cpp_text - 2) != L'\\')
                    {
                    ++cpp_text;
                    continue;
                    }
                // skip quote symbol that is actually inside of single quote
                if (cpp_text > m_file_start + 1 &&
                    *(cpp_text - 1) == L'\'' &&
                    *(cpp_text + 1) == L'\'')
                    {
                    ++cpp_text;
                    continue;
                    }
                // see if this string is in a function call or is a direct variable assignment
                // and gauge whether it is meant to be translatable or not
                std::wstring functionName, variableName, variableType;
                const wchar_t* startPos = cpp_text - 1;
                const wchar_t* functionVarNamePos = nullptr;
                // step back over 'L' symbol and spaces in front of quote
                if (*startPos == L'L')
                    { --startPos; }
                while (startPos > m_file_start &&
                        std::iswspace(*startPos))
                    { --startPos; }
                // if we are on a character that can be part of a variable
                // (and not punctuation) at this point,
                // then this might be #defined variable
                if (is_valid_name_char(*startPos))
                    {
                    const wchar_t* directiveStart = startPos;
                    while (directiveStart > m_file_start &&
                            is_valid_name_char(*directiveStart) )
                        { --directiveStart; }
                    variableName.assign(directiveStart+1, startPos - directiveStart);
                    }
                else
                    {
                    functionVarNamePos =
                        read_var_or_function_name(startPos, m_file_start, functionName,
                                                  variableName, variableType);
                    }
                // find the end of the string now and feed it into the system
                ++cpp_text;
                wchar_t* end = cpp_text;
                while (end && end < endSentinel)
                    {
                    end = std::wcschr(end, L'\"');
                    if (end)
                        {
                        // Watch out for escaped quotes.
                        // Note that we could have escaped slashes in front of a quote, so
                        // make sure that they slash in front of the quote (if there is one)
                        // is intended for it.
                        size_t proceedingSlashCount(0);
                        const wchar_t* proceedingSlashes = end-1;
                        while (proceedingSlashes >= m_file_start &&
                            *proceedingSlashes == L'\\')
                            {
                            --proceedingSlashes;
                            ++proceedingSlashCount;
                            }
                        // something like "hello\\\" there" should be seen as "hello\" there".
                        if (proceedingSlashCount > 0 &&
                            ((proceedingSlashCount % 2) != 0) )
                            {
                            ++end;
                            continue;
                            }
                        if (variableName.length())
                            {
                            process_variable(variableType, variableName,
                                std::wstring(cpp_text, end - cpp_text), (cpp_text - m_file_start));
                            }
                        else if (functionName.length())
                            {
                            if (is_diagnostic_function(functionName))
                                {
                                m_internal_strings.emplace_back(string_info(
                                    std::wstring(cpp_text, end - cpp_text),
                                    string_info::usage_info(
                                        string_info::usage_info::usage_type::function,
                                        functionName),
                                    m_file_name, get_line_and_column(cpp_text - m_file_start)));
                                }
                            else if (m_localization_functions.find(functionName) !=
                                m_localization_functions.cend())
                                {
                                m_localizable_strings.emplace_back(string_info(
                                    std::wstring(cpp_text, end - cpp_text),
                                    string_info::usage_info(
                                        string_info::usage_info::usage_type::function,
                                        functionName),
                                    m_file_name, get_line_and_column(cpp_text - m_file_start)));

                                assert(functionVarNamePos);
                                if (functionVarNamePos)
                                    {
                                    std::wstring functionNameOuter, variableNameOuter, variableTypeOuter;
                                    read_var_or_function_name(functionVarNamePos, m_file_start,
                                                              functionNameOuter, variableNameOuter,
                                                              variableTypeOuter);
                                    // internal functions
                                    if (m_internal_functions.find(functionNameOuter) !=
                                        m_internal_functions.cend() ||
                                        // CTORs whose arguments should not be translated
                                        m_untranslatable_variable_types.find(functionNameOuter) !=
                                        m_untranslatable_variable_types.cend())
                                        {
                                        m_localizable_strings_in_internal_call.emplace_back(
                                            string_info(std::wstring(cpp_text, end - cpp_text),
                                            string_info::usage_info(
                                                string_info::usage_info::usage_type::function,
                                                functionNameOuter),
                                            m_file_name,
                                            get_line_and_column(cpp_text - m_file_start)));
                                        }
                                    // untranslatable variable types
                                    else if (m_untranslatable_variable_types.find(variableTypeOuter) !=
                                        m_untranslatable_variable_types.cend())
                                        {
                                        m_localizable_strings_in_internal_call.emplace_back(
                                            string_info(std::wstring(cpp_text, end - cpp_text),
                                            string_info::usage_info(
                                                string_info::usage_info::usage_type::variable,
                                                variableNameOuter, variableTypeOuter),
                                            m_file_name,
                                            get_line_and_column(cpp_text - m_file_start)));
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
                                                        string_info(std::wstring(cpp_text, end-cpp_text),
                                                        string_info::usage_info(
                                                            string_info::usage_info::usage_type::variable,
                                                            variableNameOuter, variableTypeOuter),
                                                        m_file_name,
                                                        get_line_and_column(cpp_text - m_file_start)));
                                                    break;
                                                    }
                                                }
                                            }
                                        catch (const std::exception& exp)
                                            { log_message(variableNameOuter, exp.what()); }
                                        }
                                    }
                                }
                            else if (m_non_localizable_functions.find(functionName) !=
                                m_non_localizable_functions.cend())
                                {
                                m_marked_as_non_localizable_strings.emplace_back(
                                    string_info(std::wstring(cpp_text, end - cpp_text),
                                    string_info::usage_info(
                                        string_info::usage_info::usage_type::function,
                                        functionName),
                                    m_file_name,
                                    get_line_and_column(cpp_text - m_file_start)));
                                }
                            else if (m_untranslatable_variable_types.find(functionName) !=
                                m_untranslatable_variable_types.cend())
                                {
                                m_internal_strings.emplace_back(
                                    string_info(std::wstring(cpp_text, end - cpp_text),
                                    string_info::usage_info(
                                        string_info::usage_info::usage_type::function,
                                        functionName),
                                    m_file_name,
                                    get_line_and_column(cpp_text - m_file_start)));
                                }
                            else if (is_keyword(functionName))
                                {
                                classify_non_localizable_string(
                                    string_info(std::wstring(cpp_text, end - cpp_text),
                                    string_info::usage_info(
                                        string_info::usage_info::usage_type::orphan,
                                        L""),
                                    m_file_name,
                                    get_line_and_column(cpp_text - m_file_start)));
                                }
                            else
                                {
                                classify_non_localizable_string(
                                    string_info(std::wstring(cpp_text, end - cpp_text),
                                    string_info::usage_info(
                                        string_info::usage_info::usage_type::function,
                                        functionName),
                                    m_file_name,
                                    get_line_and_column(cpp_text - m_file_start)));
                                }
                            }
                        else
                            {
                            classify_non_localizable_string(
                                string_info(std::wstring(cpp_text, end - cpp_text),
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::orphan,
                                    L""),
                                m_file_name,
                                get_line_and_column(cpp_text-m_file_start)));
                            } 
                        clear_section(cpp_text,end + 1);
                        cpp_text = end+1;
                        break;
                        }
                    else
                        { break; }
                    }
                }
            else
                { ++cpp_text; }
            }

        m_file_name.clear();
        }

    //--------------------------------------------------
    void cpp_i18n_review::remove_decorations(std::wstring& str) const
        {
        if (str.length() && str.back() == L'>')
            {
            const auto templateStart = str.find_last_of(L'<');
            if (templateStart != std::wstring::npos)
                { str.erase(templateStart); }
            }
        // strip off colons in front of string (e.g., the common practice of typing "::" in front
        // of items in the global namespace). Also get rid of any accessors (e.g., '>' (from "->") or '.').
        if (str.length() &&
            (str.front() == L':' || str.front() == L'>' || str.front() == L'.'))
            {
            const auto colonEnd = str.find_first_not_of(L":>.");
            str.erase(0, (colonEnd == std::wstring::npos) ? str.length() : colonEnd);
            }
        // lop off name of variable from member call (e.g., "str.Format" becomes "Format").
        const auto accessor = str.find_first_of(L">.");
        if (accessor != std::wstring::npos)
            { str.erase(0,accessor+1); }
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::process_assembly_block(wchar_t* asmStart)
        {
        assert(asmStart);
        wchar_t* originalStart = asmStart;
        // GCC
        if (std::wcsncmp(asmStart, L"asm", 3) == 0 ||
            std::wcsncmp(asmStart, L"__asm__", 7) == 0)
            {
            asmStart += (std::wcsncmp(asmStart, L"asm", 3) == 0) ? 3 : 7;
            // step over spaces between __asm and its content
            while (*asmStart && std::iswspace(*asmStart))
                { ++asmStart; }
            // skip (optional) volatile modifier
            if (std::wcsncmp(asmStart, L"volatile", 8) == 0)
                {
                asmStart += 8;
                while (*asmStart && std::iswspace(*asmStart))
                    { ++asmStart; }
                }
            else if (std::wcsncmp(asmStart, L"__volatile__", 12) == 0)
                {
                asmStart += 12;
                while (*asmStart && std::iswspace(*asmStart))
                    { ++asmStart; }
                }
            if (*asmStart == L'(')
                {
                auto end = string_util::find_matching_close_tag(asmStart + 1, L'(', L')');
                if (!end)
                    {
                    log_message(L"asm", "Missing closing ')' in asm block.");
                    return asmStart + 1;
                    }
                clear_section(originalStart, const_cast<wchar_t*>(end) + 1);
                return const_cast<wchar_t*>(end) + 1;
                }
            else
                {
                const size_t end = std::wcscspn(asmStart, L"\n\r");
                clear_section(originalStart, asmStart + end + 1);
                return asmStart + end + 1;
                }
            }
        // MSVC
        if (std::wcsncmp(asmStart, L"__asm", 5) == 0)
            {
            asmStart += 5;
            // step over spaces between __asm and its content
            while (*asmStart && std::iswspace(*asmStart))
                { ++asmStart; }
            if (*asmStart == L'{')
                {
                auto end = string_util::find_matching_close_tag(asmStart+1, L'{', L'}');
                if (!end)
                    {
                    log_message(L"__asm", "Missing closing '}' in __asm block.");
                    return asmStart + 1;
                    }
                clear_section(originalStart, (end+1));
                return const_cast<wchar_t*>(end) + 1;
                }
            else
                {
                const size_t end = std::wcscspn(asmStart, L"\n\r");
                clear_section(originalStart, asmStart + end + 1);
                return asmStart + end + 1;
                }
            }
        return asmStart+1;
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::process_preprocessor_directive(wchar_t* directiveStart,
                                                             const size_t directivePos)
        {
        assert(directiveStart);
        wchar_t* originalStart = directiveStart;

        if (*directiveStart == L'#')
            { ++directiveStart; }
        // step over spaces between '#' and its directive (e.g., pragma)
        while (*directiveStart &&
            string_util::is_either(*directiveStart, L' ', L'\t'))
            { ++directiveStart; }
        // skip single-line directives
        if (std::wcsncmp(directiveStart, L"pragma", 6) == 0 ||
            std::wcsncmp(directiveStart, L"include", 7) == 0)
            {
            const size_t end = std::wcscspn(directiveStart, L"\n\r");
            clear_section(originalStart, directiveStart+end+1);
            return directiveStart+end+1;
            }
        else if (std::wcsncmp(directiveStart, L"if",2) == 0 ||
                 std::wcsncmp(directiveStart, L"ifdef",5) == 0 ||
                 std::wcsncmp(directiveStart, L"ifndef",6) == 0 ||
                 std::wcsncmp(directiveStart, L"else",4) == 0 ||
                 std::wcsncmp(directiveStart, L"elif",4) == 0 ||
                 std::wcsncmp(directiveStart, L"endif",5) == 0 ||
                 std::wcsncmp(directiveStart, L"undef",5) == 0 ||
                 std::wcsncmp(directiveStart, L"define",6) == 0)
            {
            wchar_t* end = directiveStart;
            while (*end != 0)
                {
                if (*end == L'\n' || *end == L'\r')
                    {
                    // At end of line?
                    // Make sure this isn't a multi-line directive before stopping.
                    bool multiLine = false;
                    wchar_t* backTrace = end;
                    while (backTrace > directiveStart)
                        {
                        if (std::iswspace(*backTrace))
                            {
                            --backTrace;
                            continue;
                            }
                        else if (*backTrace == L'\\')
                            {
                            multiLine = true;
                            break;
                            }
                        else
                            { break; }
                        }
                    // Just a single-line (or at the last line)? Then we are done.
                    if (!multiLine)
                        { break; }
                    }
                ++end;
                }
            // special parsing logic for #define sections
            // (try to review strings in here as best we can)
            if (std::wcsncmp(directiveStart, L"define", 6) == 0)
                {
                directiveStart += 6;
                while (*directiveStart && string_util::is_either(*directiveStart,L' ', L'\t'))
                    { ++directiveStart; }
                wchar_t* endOfDefinedTerm = directiveStart;
                while (endOfDefinedTerm < end && *endOfDefinedTerm != 0 &&
                    is_valid_name_char(*endOfDefinedTerm))
                    { ++endOfDefinedTerm; }
                const std::wstring definedTerm =
                    std::wstring(directiveStart, endOfDefinedTerm - directiveStart);

                directiveStart = endOfDefinedTerm+1;
                while (*directiveStart &&
                    (*directiveStart == L' ' || *directiveStart == L'\t' || *directiveStart == L'('))
                    { ++directiveStart; }
                auto endOfPossibleFuncName = directiveStart;
                while (is_valid_name_char(*endOfPossibleFuncName))
                    { ++endOfPossibleFuncName; }
                if (*endOfPossibleFuncName == L'(' &&
                    m_ctors_to_ignore.find(
                        std::wstring(directiveStart, endOfPossibleFuncName - directiveStart)) !=
                    m_ctors_to_ignore.end())
                    { directiveStart = endOfPossibleFuncName+1; }
                // #defined variable followed by a quote? Process as a string variable then.
                if (*directiveStart &&
                    (*directiveStart == L'\"' ||
                     (directiveStart[1] == L'\"') ))
                    {
                    auto quoteEnd = string_util::find_unescaped_char(directiveStart + 1, L'\"');
                    if (quoteEnd && (quoteEnd-directiveStart) > 0)
                        {
                        const std::wstring definedValue =
                            std::wstring(directiveStart + 1, (quoteEnd - directiveStart) - 1);
                        process_variable(L"", definedTerm,
                                    definedValue, directivePos+(directiveStart-originalStart));
                        }
                    }
                // example: #define VALUE height, #define VALUE 0x5
                // No open parentheses after the defined value--then not a function.
                // Just leave the end marker where it is (EOL) and gobble all of this up.
                else if (!string_util::strnchr(directiveStart, L'(', end-directiveStart))
                    { /*noop*/ }
                // ...or more like a #defined function, so let main parser deal with it
                // (just strip out the preprocessor junk here)
                else
                    { end = directiveStart; }
                }
            clear_section(originalStart, end);
            return end;
            }
        // unknown preprocessor, just skip the '#'
        else
            { return directiveStart+1; }
        }
    }