///////////////////////////////////////////////////////////////////////////////
// Name:        cpp_i18n_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2023 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "cpp_i18n_review.h"

using namespace i18n_string_util;

namespace i18n_check
    {
    //--------------------------------------------------
    void cpp_i18n_review::operator()(const std::wstring_view src_text,
        const std::wstring& file_name)
        {
        m_file_name = file_name;

        if (src_text.length() == 0)
            { return; }

        auto cppBuffer = std::make_unique<wchar_t[]>(src_text.length() + 1);
        wchar_t* cpp_text = cppBuffer.get();
        std::wcsncpy(cpp_text, src_text.data(), src_text.length());

        load_id_assignments(src_text, file_name);
        load_deprecated_functions(src_text, file_name);

        m_file_start = cpp_text;
        const wchar_t* const endSentinel = cpp_text + src_text.length();

        while (cpp_text && cpp_text + 1 < endSentinel && cpp_text[0] != 0)
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
                        clear_section(cpp_text, end + 2);
                        cpp_text = end + 2;
                        }
                    // can't find ending tag, so just read in the rest of the text
                    else
                        { return; }
                    }
                // or a single line comment
                else if (cpp_text[1] == L'/' &&
                    cpp_text + 2 < endSentinel)
                    {
                    const size_t end = std::wcscspn(cpp_text, L"\n\r");
                    if ((m_reviewStyles & check_space_after_comment) &&
                        std::iswalnum(cpp_text[2]) &&
                        // something like "//--------" is OK
                        cpp_text[2] != L'-')
                        {
                        m_comments_missing_space.push_back(
                            string_info(std::wstring{},
                                string_info::usage_info{},
                                m_file_name, get_line_and_column((cpp_text - m_file_start))));
                        }
                    clear_section(cpp_text, cpp_text + end);
                    cpp_text += end;
                    }
                // not a comment
                else
                    { ++cpp_text; }
                }
            else if (cpp_text[0] == L'#')
                {
                cpp_text = process_preprocessor_directive(cpp_text, cpp_text - m_file_start);
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
                std::wstring functionName, variableName,
                             variableType, deprecatedMacroEncountered;
                size_t parameterPosition{ 0 };
                const wchar_t* startPos = cpp_text - 1;
                const wchar_t* functionVarNamePos{ nullptr };
                bool isRawString{ false};
                // if a raw string, step over 'R'
                if (*startPos == L'R')
                    {
                    isRawString = true;
                    --startPos;
                    }
                // step back over double-byte, u16, or u32 prefixes
                if (*startPos == L'L' ||
                    *startPos == L'u' ||
                    *startPos == L'U')
                    { --startPos; }
                // step back over UTF-8 'u8' symbol
                if (startPos > m_file_start + 1 &&
                    *startPos == L'8' &&
                    *(startPos - 1) != L'u')
                    { startPos -= 2; }
                // ...and spaces in front of quote
                while (startPos > m_file_start &&
                        std::iswspace(*startPos))
                    { --startPos; }
                // if we are on a character that can be part of a variable
                // (and not punctuation) at this point,
                // then this might be a #defined variable
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
                                                  variableName, variableType, deprecatedMacroEncountered,
                                                  parameterPosition);
                    }
                // find the end of the string now and feed it into the system
                ++cpp_text;
                wchar_t* end = cpp_text;
                if (isRawString)
                    { end = std::wcsstr(end, L")\""); }
                else
                    {
                    while (end && end < endSentinel)
                        {
                        end = std::wcschr(end, L'\"');
                        if (end)
                            {
                            // Watch out for escaped quotes.
                            // Note that we could have escaped slashes in front of a quote, so
                            // make sure that they slash in front of the quote (if there is one)
                            // is intended for it.
                            size_t proceedingSlashCount{ 0 };
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

                            // Format macros for the std::fprintf family of functions that may
                            // appear between quoted sections that will actually join the two quotes
                            const std::wregex intPrintfMacro
                                { LR"(PR[IN][uidoxX](8|16|32|64|FAST8|FAST16|FAST32|FAST64|LEAST8|LEAST16|LEAST32|LEAST64|MAX|PTR))" };
                            constexpr size_t int64PrintfMacroLength{ 6 };
                            // see if there is more to this string on another line
                            wchar_t* connectedQuote = end + 1;
                            while (connectedQuote < endSentinel &&
                                std::iswspace(*connectedQuote))
                                { ++connectedQuote; }
                            if (connectedQuote < endSentinel &&
                                *connectedQuote == L'\"')
                                {
                                end = connectedQuote + 1;
                                continue;
                                }
                            // step over PRIu64 macro that appears between printf strings
                            else if (connectedQuote + int64PrintfMacroLength < endSentinel &&
                                std::regex_match(
                                    std::wstring{ connectedQuote, int64PrintfMacroLength },
                                    intPrintfMacro))
                                {
                                clear_section(connectedQuote,
                                              connectedQuote + int64PrintfMacroLength);
                                connectedQuote += int64PrintfMacroLength;
                                while (connectedQuote < endSentinel &&
                                    std::iswspace(*connectedQuote))
                                    { ++connectedQuote; }
                                if (connectedQuote < endSentinel &&
                                    *connectedQuote == L'\"')
                                    {
                                    end = connectedQuote + 1;
                                    continue;
                                    }
                                }
                            break;
                            }
                        else
                            { break; }
                        }
                    }

                // if we found the end of the quote
                if (end && end < endSentinel)
                    {
                    process_quote(cpp_text, end, functionVarNamePos, variableName, functionName,
                                  variableType, deprecatedMacroEncountered, parameterPosition);
                    cpp_text = end + (isRawString ? 2 : 1);
                    }
                }
            // ";}" should have a space or newline between them
            else if (cpp_text[0] == L';' && cpp_text[1] == L'}')
                {
                log_message(L"MISSING SPACE",
                        L"Space or newline should be inserted between ';' and '}'.",
                        (cpp_text - m_file_start));
                ++cpp_text;
                }
            else
                {
                if ((m_reviewStyles & check_tabs) &&
                    cpp_text[0] == L'\t')
                    {
                    m_tabs.push_back(
                        string_info(std::wstring{},
                                string_info::usage_info{},
                                m_file_name, get_line_and_column((cpp_text - m_file_start))));
                    }
                else if ((m_reviewStyles & check_trailing_spaces) &&
                    cpp_text[0] == L' ' && cpp_text + 1 < endSentinel &&
                    (cpp_text[1] == L'\n' || cpp_text[1] == L'\r'))
                    {
                    assert(cpp_text >= m_file_start);
                    auto prevLineStart = string_util::find_last_of(m_file_start, L"\n\r",
                                                                   cpp_text - m_file_start);
                    if (prevLineStart == std::wstring::npos)
                        { prevLineStart = 0; }
                    // step forward to original line
                    ++prevLineStart;
                    std::wstring codeLine(m_file_start + prevLineStart,
                                          (cpp_text - (m_file_start + prevLineStart)));
                    string_util::ltrim(codeLine);
                    m_trailing_spaces.push_back(
                        string_info(codeLine,
                            string_info::usage_info{},
                            m_file_name, get_line_and_column((cpp_text - m_file_start))));
                    }
                else if ((m_reviewStyles & check_line_width) &&
                    (cpp_text[0] == L'\n' || cpp_text[0] == L'\r') &&
                    cpp_text > m_file_start)
                    {
                    const auto currentPos{ (cpp_text - m_file_start) };
                    auto previousNewLine =
                        string_util::find_last_of(m_file_start, L"\n\r", currentPos - 1);
                    if (previousNewLine == std::wstring::npos)
                        { previousNewLine = 0; }
                    const size_t currentLineLength{ currentPos - (++previousNewLine) };
                    if (currentLineLength > m_max_line_length)
                        {
                        // ...also, only warn if the current line doesn't have a raw
                        // string in it--those can make it complicated to break a line
                        // into smaller lines.
                        // We will also ignore the line if it appears to be a long bitmask.
                        const std::wstring_view currentLine{ m_file_start + previousNewLine, currentLineLength };
                        if (currentLine.find(L"R\"") == std::wstring::npos &&
                            currentLine.find(L"|") == std::wstring::npos)
                            {
                            m_wide_lines.push_back(
                                // truncate and add ellipsis
                                string_info(std::wstring{ currentLine.substr(0, 32 ) }.append(L"..."),
                                    string_info::usage_info{
                                        string_info::usage_info::usage_type::orphan,
                                        std::to_wstring(currentLineLength),
                                        std::wstring{}
                                        },
                                    m_file_name, get_line_and_column(currentPos)));
                            }
                        }
                    }
                ++cpp_text;
                }
            }

        m_file_name.clear();
        }

    //--------------------------------------------------
    void cpp_i18n_review::remove_decorations(std::wstring& str) const
        {
        while (str.length() && str.back() == L'&')
            { str.pop_back(); }
        if (str.length() && str.back() == L'>')
            {
            const auto templateStart = str.find_last_of(L'<');
            if (templateStart != std::wstring::npos)
                {
                // if constructing a shared_ptr, then use the type that it is constructing
                if (std::wstring_view{ str }.substr(0, templateStart).compare(L"std::make_shared") == 0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"make_shared") == 0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"std::shared_ptr") == 0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"shared_ptr") == 0)
                    {
                    str.erase(0, templateStart + 1);
                    str.pop_back();
                    }
                // use the root type of the variable with template info stripped off
                else
                    { str.erase(templateStart); }
                }
            }
        // Strip off colons in front of string (e.g., the common practice of typing "::" in front
        // of items in the global namespace).
        // Also get rid of any accessors (e.g., '>' (from "->") or '.').
        if (str.length() &&
            (str.front() == L':' || str.front() == L'>' || str.front() == L'.'))
            {
            const auto colonEnd = str.find_first_not_of(L":>.");
            str.erase(0, (colonEnd == std::wstring::npos) ? str.length() : colonEnd);
            }
        // lop off name of variable from member call (e.g., "str.Format" becomes "Format").
        const auto accessor = str.find_first_of(L">.");
        if (accessor != std::wstring::npos)
            { str.erase(0, accessor + 1); }
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
                    log_message(L"asm", L"Missing closing ')' in asm block.", (end - m_file_start));
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
                    log_message(L"__asm", L"Missing closing '}' in __asm block.", (end - m_file_start));
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
    wchar_t* cpp_i18n_review::skip_preprocessor_define_block(wchar_t* directiveStart)
        {
        const std::wregex debugRE{ L"[_]*DEBUG[_]*" };
        const std::wregex debugLevelRE{ L"([a-zA-Z_]*DEBUG_LEVEL|0)" };
        const std::wregex releaseRE{ L"[_]*RELEASE[_]*" };
        const auto findSectionEnd = [](wchar_t* sectionStart)
            {
            auto closingElIf = string_util::find_matching_close_tag(sectionStart, L"#if", L"#elif");
            auto closingEndIf = string_util::find_matching_close_tag(sectionStart, L"#if", L"#endif");
            if (closingElIf != nullptr && closingEndIf != nullptr)
                {
                if (closingElIf < closingEndIf)
                    {
                    auto pDiff{ closingElIf - sectionStart };
                    return sectionStart + pDiff + 5;
                    }
                else
                    {
                    auto pDiff{ closingEndIf - sectionStart };
                    return sectionStart + pDiff + 6;
                    }
                }
            else if (closingElIf != nullptr)
                {
                auto pDiff{ closingElIf - sectionStart };
                return sectionStart + pDiff + 5;
                }
            else
                {
                auto pDiff{ closingEndIf - sectionStart };
                return sectionStart + pDiff + 6;
                }
            };

        if (std::wcsncmp(directiveStart, L"ifndef", 6) == 0)
            {
            directiveStart += 6;
            while (std::iswspace(*directiveStart))
                { ++directiveStart; }
            auto defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                { ++defSymbolEnd; }
            std::wstring defSymbol{ directiveStart,
                                    static_cast<size_t>(defSymbolEnd - directiveStart) };
            // NDEBUG (i.e., release) is a standard symbol;
            // if not defined, then this is a debug preprocessor section
            return (defSymbol.compare(L"NDEBUG") == 0 ||
                    std::regex_match(defSymbol, releaseRE)) ?
                findSectionEnd(defSymbolEnd) : nullptr;
            }
        else if (std::wcsncmp(directiveStart, L"ifdef", 5) == 0)
            {
            directiveStart += 5;
            while (std::iswspace(*directiveStart))
                { ++directiveStart; }
            auto defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                { ++defSymbolEnd; }
            std::wstring defSymbol{ directiveStart,
                                    static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugRE)) ?
                findSectionEnd(defSymbolEnd) : nullptr;
            }
        else if (std::wcsncmp(directiveStart, L"if defined", 10) == 0)
            {
            directiveStart += 10;
            while (std::iswspace(*directiveStart))
                { ++directiveStart; }
            auto defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                { ++defSymbolEnd; }
            std::wstring defSymbol{ directiveStart,
                                    static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugRE)) ?
                findSectionEnd(defSymbolEnd) : nullptr;
            }
        else if (std::wcsncmp(directiveStart, L"if", 2) == 0)
            {
            directiveStart += 2;
            while (std::iswspace(*directiveStart))
                { ++directiveStart; }
            auto defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                { ++defSymbolEnd; }
            std::wstring defSymbol{ directiveStart,
                                    static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugLevelRE)) ?
                findSectionEnd(defSymbolEnd) : nullptr;
            }
        else
            { return nullptr; }
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

        const auto blockEnd = skip_preprocessor_define_block(directiveStart);
        if (blockEnd != nullptr)
            {
            clear_section(directiveStart, blockEnd);
            return blockEnd;
            }

        // skip single-line directives
        if (std::wcsncmp(directiveStart, L"pragma", 6) == 0 ||
            std::wcsncmp(directiveStart, L"include", 7) == 0)
            {
            const size_t end = std::wcscspn(directiveStart, L"\n\r");
            clear_section(originalStart, directiveStart+end+1);
            return directiveStart+end+1;
            }
        else if (std::wcsncmp(directiveStart, L"if", 2) == 0 ||
                 std::wcsncmp(directiveStart, L"ifdef", 5) == 0 ||
                 std::wcsncmp(directiveStart, L"ifndef", 6) == 0 ||
                 std::wcsncmp(directiveStart, L"else", 4) == 0 ||
                 std::wcsncmp(directiveStart, L"elif", 4) == 0 ||
                 std::wcsncmp(directiveStart, L"endif", 5) == 0 ||
                 std::wcsncmp(directiveStart, L"undef", 5) == 0 ||
                 std::wcsncmp(directiveStart, L"define", 6) == 0 ||
                 std::wcsncmp(directiveStart, L"error", 5) == 0 ||
                 std::wcsncmp(directiveStart, L"warning", 7) == 0)
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
                    m_ctors_to_ignore.cend())
                    { directiveStart = endOfPossibleFuncName+1; }
                // #define'd variable followed by a quote? Process as a string variable then.
                if (*directiveStart &&
                    (*directiveStart == L'\"' ||
                     (directiveStart[1] == L'\"') ))
                    {
                    auto quoteEnd = string_util::find_unescaped_char(directiveStart + 1, L'\"');
                    if (quoteEnd && (quoteEnd-directiveStart) > 0)
                        {
                        const std::wstring definedValue =
                            std::wstring(directiveStart + 1, (quoteEnd - directiveStart) - 1);
                        process_variable(std::wstring{}, definedTerm,
                                    definedValue, directivePos+(directiveStart-originalStart));
                        }
                    }
                // example: #define VALUE height, #define VALUE 0x5
                // No open parentheses after the defined value--then not a function.
                // Just leave the end marker where it is (EOL) and gobble all of this up.
                else if (!string_util::strnchr(directiveStart, L'(', end-directiveStart))
                    { /*no-op*/ }
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