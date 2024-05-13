///////////////////////////////////////////////////////////////////////////////
// Name:        cpp_i18n_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "cpp_i18n_review.h"

namespace i18n_check
    {
    //--------------------------------------------------
    void cpp_i18n_review::operator()(const std::wstring_view srcText, const std::wstring& fileName)
        {
        m_file_name = fileName;
        m_file_start = nullptr;

        if (srcText.length() == 0)
            {
            return;
            }

        std::wstring cppBuffer{ srcText };
        wchar_t* cppText = cppBuffer.data();

        load_id_assignments(srcText, fileName);
        load_deprecated_functions(srcText, fileName);

        m_file_start = cppText;
        const wchar_t* const endSentinel =
            std::next(cppText, static_cast<ptrdiff_t>(srcText.length()));

        while (cppText != nullptr && std::next(cppText, 1) < endSentinel && *cppText != 0)
            {
            // if a possible comment, then scan past it
            if (*cppText == L'/')
                {
                // see if a block comment (/*comment*/)
                if (*std::next(cppText) == L'*')
                    {
                    wchar_t* end = std::wcsstr(cppText, L"*/");
                    if (end != nullptr && end < endSentinel)
                        {
                        clear_section(cppText, std::next(end, 2));
                        cppText = std::next(end, 2);
                        }
                    // can't find ending tag, so just read in the rest of the text
                    else
                        {
                        return;
                        }
                    }
                // or a single line comment
                else if (cppText[1] == L'/' && std::next(cppText, 2) < endSentinel)
                    {
                    const size_t endPos = std::wcscspn(cppText, L"\n\r");
                    if (static_cast<bool>(m_reviewStyles & check_space_after_comment) &&
                        static_cast<bool>(std::iswalnum(cppText[2])) &&
                        // something like "//--------" is OK
                        cppText[2] != L'-')
                        {
                        m_comments_missing_space.push_back(
                            string_info(std::wstring{}, string_info::usage_info{}, m_file_name,
                                        get_line_and_column((cppText - m_file_start))));
                        }
                    clear_section(cppText, std::next(cppText, static_cast<ptrdiff_t>(endPos)));
                    std::advance(cppText, endPos);
                    }
                // not a comment
                else
                    {
                    std::advance(cppText, 1);
                    }
                }
            else if (cppText[0] == L'#')
                {
                cppText = process_preprocessor_directive(cppText, cppText - m_file_start);
                }
            else if (((cppText == m_file_start) || !is_valid_name_char(cppText[-1])) &&
                     is_assembly_block(cppText))
                {
                cppText = process_assembly_block(cppText);
                }
            // ...or an actual quote
            else if (cppText[0] == L'\"')
                {
                // skip escaped quotes
                if (cppText > m_file_start + 1 && *(cppText - 1) == L'\\' &&
                    *(cppText - 2) != L'\\')
                    {
                    std::advance(cppText, 1);
                    continue;
                    }
                // skip quote symbol that is actually inside of single quote
                if (cppText > m_file_start + 1 && *(cppText - 1) == L'\'' &&
                    *(cppText + 1) == L'\'')
                    {
                    std::advance(cppText, 1);
                    continue;
                    }
                // see if this string is in a function call or is a direct variable assignment
                // and gauge whether it is meant to be translatable or not
                std::wstring functionName;
                std::wstring variableName;
                std::wstring variableType;
                std::wstring deprecatedMacroEncountered;
                size_t parameterPosition{ 0 };
                const wchar_t* startPos = std::prev(cppText, 1);
                const wchar_t* functionVarNamePos{ nullptr };
                bool isRawString{ false };
                // if a raw string, step over 'R'
                if (*startPos == L'R')
                    {
                    isRawString = true;
                    std::advance(startPos, -1);
                    }
                // step back over double-byte, u16, or u32 prefixes
                if (*startPos == L'L' || *startPos == L'u' || *startPos == L'U')
                    {
                    std::advance(startPos, -1);
                    }
                // step back over UTF-8 'u8' symbol
                if (startPos > m_file_start + 1 && *startPos == L'8' && *(startPos - 1) != L'u')
                    {
                    std::advance(startPos, -2);
                    }
                // ...and spaces in front of quote
                while (startPos > m_file_start && static_cast<bool>(std::iswspace(*startPos)))
                    {
                    std::advance(startPos, -1);
                    }
                // if we are on a character that can be part of a variable
                // (and not punctuation) at this point,
                // then this might be a #defined variable
                if (is_valid_name_char(*startPos))
                    {
                    const wchar_t* directiveStart = startPos;
                    while (directiveStart > m_file_start && is_valid_name_char(*directiveStart))
                        {
                        std::advance(directiveStart, -1);
                        }
                    variableName.assign(directiveStart + 1, startPos - directiveStart);
                    }
                else
                    {
                    functionVarNamePos = read_var_or_function_name(
                        startPos, m_file_start, functionName, variableName, variableType,
                        deprecatedMacroEncountered, parameterPosition);
                    }
                // find the end of the string now and feed it into the system
                std::advance(cppText, 1);
                wchar_t* end = cppText;
                if (isRawString)
                    {
                    end = std::wcsstr(end, L")\"");
                    }
                else
                    {
                    while (end != nullptr && end < endSentinel)
                        {
                        end = std::wcschr(end, L'\"');
                        if (end != nullptr)
                            {
                            // Watch out for escaped quotes.
                            // Note that we could have escaped slashes in front of a quote, so
                            // make sure that they slash in front of the quote (if there is one)
                            // is intended for it.
                            size_t proceedingSlashCount{ 0 };
                            const wchar_t* proceedingSlashes = end - 1;
                            while (proceedingSlashes >= m_file_start && *proceedingSlashes == L'\\')
                                {
                                --proceedingSlashes;
                                ++proceedingSlashCount;
                                }
                            // something like "hello\\\" there" should be seen as "hello\" there".
                            if (proceedingSlashCount > 0 && ((proceedingSlashCount % 2) != 0))
                                {
                                ++end;
                                continue;
                                }

                            // Format macros for the std::fprintf family of functions that may
                            // appear between quoted sections that will actually join the two quotes
                            const std::wregex intPrintfMacro{
                                LR"(PR[IN][uidoxX](8|16|32|64|FAST8|FAST16|FAST32|FAST64|LEAST8|LEAST16|LEAST32|LEAST64|MAX|PTR))"
                            };
                            constexpr size_t int64PrintfMacroLength{ 6 };
                            // see if there is more to this string on another line
                            wchar_t* connectedQuote = end + 1;
                            while (connectedQuote < endSentinel &&
                                   static_cast<bool>(std::iswspace(*connectedQuote)))
                                {
                                std::advance(connectedQuote, 1);
                                }
                            if (connectedQuote < endSentinel && *connectedQuote == L'\"')
                                {
                                end = std::next(connectedQuote);
                                continue;
                                }
                            // step over PRIu64 macro that appears between printf strings
                            if (connectedQuote + int64PrintfMacroLength < endSentinel &&
                                std::regex_match(
                                    std::wstring{ connectedQuote, int64PrintfMacroLength },
                                    intPrintfMacro))
                                {
                                clear_section(connectedQuote,
                                              connectedQuote + int64PrintfMacroLength);
                                connectedQuote += int64PrintfMacroLength;
                                while (connectedQuote < endSentinel &&
                                       static_cast<bool>(std::iswspace(*connectedQuote)))
                                    {
                                    std::advance(connectedQuote, 1);
                                    }
                                if (connectedQuote < endSentinel && *connectedQuote == L'\"')
                                    {
                                    end = std::next(connectedQuote);
                                    continue;
                                    }
                                }
                            break;
                            }
                        break;
                        }
                    }

                // if we found the end of the quote
                if (end != nullptr && end < endSentinel)
                    {
                    process_quote(cppText, end, functionVarNamePos, variableName, functionName,
                                  variableType, deprecatedMacroEncountered, parameterPosition);
                    cppText = std::next(end, (isRawString ? 2 : 1));
                    }
                }
            // ";}" should have a space or newline between them
            else if (*cppText == L';' && *std::next(cppText) == L'}')
                {
                log_message(L"MISSING SPACE",
                            L"Space or newline should be inserted between ';' and '}'.",
                            (cppText - m_file_start));
                std::advance(cppText, 1);
                }
            else
                {
                if (static_cast<bool>(m_reviewStyles & check_tabs) && *cppText == L'\t')
                    {
                    m_tabs.push_back(string_info(std::wstring{}, string_info::usage_info{},
                                                 m_file_name,
                                                 get_line_and_column((cppText - m_file_start))));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_trailing_spaces) &&
                         *cppText == L' ' && std::next(cppText) < endSentinel &&
                         (*std::next(cppText) == L'\n' || *std::next(cppText) == L'\r'))
                    {
                    assert(cppText >= m_file_start);
                    auto prevLineStart = std::wstring_view{ m_file_start }.find_last_of(
                        L"\n\r", cppText - m_file_start);
                    if (prevLineStart == std::wstring::npos)
                        {
                        prevLineStart = 0;
                        }
                    // step forward to original line
                    ++prevLineStart;
                    std::wstring codeLine(m_file_start + prevLineStart,
                                          (cppText - (m_file_start + prevLineStart)));
                    string_util::ltrim(codeLine);
                    m_trailing_spaces.push_back(
                        string_info(codeLine, string_info::usage_info{}, m_file_name,
                                    get_line_and_column((cppText - m_file_start))));
                    }
                else if (static_cast<bool>(m_reviewStyles & check_line_width) &&
                         (*cppText == L'\n' || *cppText == L'\r') && cppText > m_file_start)
                    {
                    const auto currentPos{ (cppText - m_file_start) };
                    auto previousNewLine =
                        std::wstring_view{ m_file_start }.find_last_of(L"\n\r", currentPos - 1);
                    if (previousNewLine == std::wstring::npos)
                        {
                        previousNewLine = 0;
                        }
                    const size_t currentLineLength{ currentPos - (++previousNewLine) };
                    if (currentLineLength > m_max_line_length)
                        {
                        // ...also, only warn if the current line doesn't have a raw
                        // string in it--those can make it complicated to break a line
                        // into smaller lines.
                        // We will also ignore the line if it appears to be a long bitmask.
                        const std::wstring_view currentLine{
                            std::next(m_file_start, static_cast<ptrdiff_t>(previousNewLine)),
                            currentLineLength
                        };
                        if (currentLine.find(L"R\"") == std::wstring::npos &&
                            currentLine.find(L'|') == std::wstring::npos)
                            {
                            m_wide_lines.emplace_back(
                                // truncate and add ellipsis
                                std::wstring{ currentLine.substr(
                                                  0, std::min<size_t>(32, currentLine.length())) }
                                    // NOLINT
                                    .append(L"..."),
                                string_info::usage_info{
                                    string_info::usage_info::usage_type::orphan,
                                    std::to_wstring(currentLineLength), std::wstring{} },
                                m_file_name, get_line_and_column(currentPos));
                            }
                        }
                    }
                std::advance(cppText, 1);
                }
            }

        m_file_name.clear();
        m_file_start = nullptr;
        }

    //--------------------------------------------------
    void cpp_i18n_review::remove_decorations(std::wstring& str) const
        {
        while (str.length() > 0 && str.back() == L'&')
            {
            str.pop_back();
            }
        if (str.length() > 0 && str.back() == L'>')
            {
            const auto templateStart = str.find_last_of(L'<');
            if (templateStart != std::wstring::npos)
                {
                // if constructing a shared_ptr, then use the type that it is constructing
                if (std::wstring_view{ str }
                            .substr(0, templateStart)
                            .compare(L"std::make_shared") == 0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"make_shared") ==
                        0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"std::shared_ptr") ==
                        0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"shared_ptr") == 0)
                    {
                    str.erase(0, templateStart + 1);
                    str.pop_back();
                    }
                // use the root type of the variable with template info stripped off
                else
                    {
                    str.erase(templateStart);
                    }
                }
            }
        // Strip off colons in front of string (e.g., the common practice of typing "::" in front
        // of items in the global namespace).
        // Also get rid of any accessors (e.g., '>' (from "->") or '.').
        if (str.length() > 0 && (str.front() == L':' || str.front() == L'>' || str.front() == L'.'))
            {
            const auto colonEnd = str.find_first_not_of(L":>.");
            str.erase(0, (colonEnd == std::wstring::npos) ? str.length() : colonEnd);
            }
        // lop off name of variable from member call (e.g., "str.Format" becomes "Format").
        const auto accessor = str.find_first_of(L">.");
        if (accessor != std::wstring::npos)
            {
            str.erase(0, accessor + 1);
            }
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::process_assembly_block(wchar_t* asmStart)
        {
        assert(asmStart);
        const std::wstring_view asmCommand1{ L"asm" };
        const std::wstring_view asmCommand2{ L"__asm__" };
        const std::wstring_view asmCommand3{ L"__asm" };
        wchar_t* const originalStart = asmStart;
        // GCC
        if (std::wcsncmp(asmStart, asmCommand1.data(), asmCommand1.length()) == 0 ||
            std::wcsncmp(asmStart, asmCommand2.data(), asmCommand2.length()) == 0)
            {
            asmStart += (std::wcsncmp(asmStart, asmCommand1.data(), asmCommand1.length()) == 0) ?
                            asmCommand1.length() :
                            asmCommand2.length();
            // step over spaces between __asm and its content
            while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                {
                std::advance(asmStart, 1);
                }
            const std::wstring_view volatileCommand1{ L"volatile" };
            const std::wstring_view volatileCommand2{ L"__volatile__" };
            // skip (optional) volatile modifier
            if (std::wcsncmp(asmStart, volatileCommand1.data(), volatileCommand1.length()) == 0)
                {
                std::advance(asmStart, volatileCommand1.length());
                while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                    {
                    std::advance(asmStart, 1);
                    }
                }
            else if (std::wcsncmp(asmStart, volatileCommand2.data(), volatileCommand2.length()) ==
                     0)
                {
                std::advance(asmStart, volatileCommand2.length());
                while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                    {
                    std::advance(asmStart, 1);
                    }
                }
            if (*asmStart == L'(')
                {
                const auto* end =
                    string_util::find_matching_close_tag(std::next(asmStart), L'(', L')', false);
                if (end == nullptr)
                    {
                    log_message(L"asm", L"Missing closing ')' in asm block.", (end - m_file_start));
                    return std::next(asmStart, 1);
                    }
                clear_section(originalStart, const_cast<wchar_t*>(end) + 1);
                return const_cast<wchar_t*>(end) + 1;
                }
            if (*asmStart != 0)
                {
                const size_t end = std::wcscspn(asmStart, L"\n\r");
                clear_section(originalStart, std::next(asmStart, static_cast<ptrdiff_t>(end + 1)));
                return std::next(asmStart, static_cast<ptrdiff_t>(end + 1));
                }
            return nullptr;
            }
        // MSVC
        if (std::wcsncmp(asmStart, asmCommand3.data(), asmCommand3.length()) == 0)
            {
            std::advance(asmStart, asmCommand3.length());
            // step over spaces between __asm and its content
            while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                {
                std::advance(asmStart, 1);
                }
            if (*asmStart == L'{')
                {
                const auto* end =
                    string_util::find_matching_close_tag(std::next(asmStart), L'{', L'}', false);
                if (end == nullptr)
                    {
                    log_message(L"__asm", L"Missing closing '}' in __asm block.",
                                (end - m_file_start));
                    return std::next(asmStart);
                    }
                clear_section(originalStart, std::next(end));
                return const_cast<wchar_t*>(end) + 1;
                }
            if (*asmStart != 0)
                {
                const size_t endPos = std::wcscspn(asmStart, L"\n\r");
                clear_section(originalStart,
                              std::next(asmStart, static_cast<ptrdiff_t>(endPos + 1)));
                return std::next(asmStart, static_cast<ptrdiff_t>(endPos + 1));
                }
            return nullptr;
            }
        return std::next(asmStart);
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::skip_preprocessor_define_block(wchar_t* directiveStart)
        {
        const std::wregex debugRE{ L"[_]*DEBUG[_]*" };
        const std::wregex debugLevelRE{ L"([a-zA-Z_]*DEBUG_LEVEL|0)" };
        const std::wregex releaseRE{ L"[_]*RELEASE[_]*" };
        const auto findSectionEnd = [](wchar_t* sectionStart) -> wchar_t*
        {
            const std::wstring_view elifCommand{ L"#elif" };
            const std::wstring_view endifCommand{ L"#endif" };
            const auto* closingElIf =
                string_util::find_matching_close_tag(sectionStart, L"#if", elifCommand);
            const auto* closingEndIf =
                string_util::find_matching_close_tag(sectionStart, L"#if", endifCommand);
            if (closingElIf != nullptr && closingEndIf != nullptr)
                {
                if (closingElIf < closingEndIf)
                    {
                    auto pDiff{ closingElIf - sectionStart };
                    return std::next(sectionStart,
                                     pDiff + static_cast<ptrdiff_t>(elifCommand.length()));
                    }
                auto pDiff{ closingEndIf - sectionStart };
                return std::next(sectionStart,
                                 pDiff + static_cast<ptrdiff_t>(endifCommand.length()));
                }
            if (closingElIf != nullptr)
                {
                auto pDiff{ closingElIf - sectionStart };
                return std::next(sectionStart,
                                 pDiff + static_cast<ptrdiff_t>(elifCommand.length()));
                }
            auto pDiff{ closingEndIf - sectionStart };
            return std::next(sectionStart, pDiff + static_cast<ptrdiff_t>(endifCommand.length()));
        };

        const std::wstring_view ifndefCommand{ L"ifndef" };
        if (std::wstring_view{ directiveStart }.starts_with(ifndefCommand))
            {
            std::advance(directiveStart, ifndefCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            // NDEBUG (i.e., release) is a standard symbol;
            // if not defined, then this is a debug preprocessor section
            return (defSymbol == L"NDEBUG" || std::regex_match(defSymbol, releaseRE)) ?
                       findSectionEnd(defSymbolEnd) :
                       nullptr;
            }
        const std::wstring_view ifdefCommand{ L"ifdef" };
        if (std::wstring_view{ directiveStart }.starts_with(ifdefCommand))
            {
            std::advance(directiveStart, ifdefCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugRE)) ? findSectionEnd(defSymbolEnd) : nullptr;
            }
        const std::wstring_view ifdefinedCommand{ L"if defined" };
        if (std::wstring_view{ directiveStart }.starts_with(ifdefinedCommand))
            {
            std::advance(directiveStart, ifdefinedCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugRE)) ? findSectionEnd(defSymbolEnd) : nullptr;
            }
        const std::wstring_view ifCommand{ L"if" };
        if (std::wstring_view{ directiveStart }.starts_with(ifCommand))
            {
            std::advance(directiveStart, ifCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugLevelRE)) ? findSectionEnd(defSymbolEnd) :
                                                                 nullptr;
            }
        return nullptr;
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::process_preprocessor_directive(wchar_t* directiveStart,
                                                             const size_t directivePos)
        {
        assert(directiveStart);
        if (directiveStart == nullptr)
            {
            return nullptr;
            }
        wchar_t* const originalStart = directiveStart;

        if (*directiveStart == L'#')
            {
            std::advance(directiveStart, 1);
            }
        // step over spaces between '#' and its directive (e.g., pragma)
        while (string_util::is_either(*directiveStart, L' ', L'\t'))
            {
            std::advance(directiveStart, 1);
            }

        auto* blockEnd = skip_preprocessor_define_block(directiveStart);
        if (blockEnd != nullptr)
            {
            clear_section(directiveStart, blockEnd);
            return blockEnd;
            }

        // skip single-line directives
        if (std::wstring_view{ directiveStart }.starts_with(L"pragma") ||
            std::wstring_view{ directiveStart }.starts_with(L"include"))
            {
            const size_t end = std::wcscspn(directiveStart, L"\n\r");
            clear_section(originalStart,
                          std::next(directiveStart, static_cast<ptrdiff_t>(end + 1)));
            return std::next(directiveStart, static_cast<ptrdiff_t>(end + 1));
            }
        if (std::wstring_view{ directiveStart }.starts_with(L"if") ||
            std::wstring_view{ directiveStart }.starts_with(L"ifdef") ||
            std::wstring_view{ directiveStart }.starts_with(L"ifndef") ||
            std::wstring_view{ directiveStart }.starts_with(L"else") ||
            std::wstring_view{ directiveStart }.starts_with(L"elif") ||
            std::wstring_view{ directiveStart }.starts_with(L"endif") ||
            std::wstring_view{ directiveStart }.starts_with(L"undef") ||
            std::wstring_view{ directiveStart }.starts_with(L"define") ||
            std::wstring_view{ directiveStart }.starts_with(L"error") ||
            std::wstring_view{ directiveStart }.starts_with(L"warning"))
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
                        if (static_cast<bool>(std::iswspace(*backTrace)))
                            {
                            std::advance(backTrace, -1);
                            continue;
                            }
                        if (*backTrace == L'\\')
                            {
                            multiLine = true;
                            break;
                            }
                        break;
                        }
                    // Just a single-line (or at the last line)? Then we are done.
                    if (!multiLine)
                        {
                        break;
                        }
                    }
                std::advance(end, 1);
                }
            // special parsing logic for #define sections
            // (try to review strings in here as best we can)
            const std::wstring defineCommand{ L"define" };
            if (std::wstring_view{ directiveStart }.starts_with(defineCommand))
                {
                std::advance(directiveStart, defineCommand.length());
                while (*directiveStart != 0 && string_util::is_either(*directiveStart, L' ', L'\t'))
                    {
                    std::advance(directiveStart, 1);
                    }
                if (*directiveStart == 0)
                    {
                    return directiveStart;
                    }
                wchar_t* endOfDefinedTerm = directiveStart;
                while (endOfDefinedTerm < end && *endOfDefinedTerm != 0 &&
                       is_valid_name_char(*endOfDefinedTerm))
                    {
                    std::advance(endOfDefinedTerm, 1);
                    }
                const std::wstring definedTerm =
                    std::wstring(directiveStart, endOfDefinedTerm - directiveStart);

                directiveStart = std::next(endOfDefinedTerm);
                while (*directiveStart != 0 &&
                       (*directiveStart == L' ' || *directiveStart == L'\t' ||
                        *directiveStart == L'('))
                    {
                    std::advance(directiveStart, 1);
                    }
                if (*directiveStart == 0)
                    {
                    return directiveStart;
                    }
                auto* endOfPossibleFuncName = directiveStart;
                while (is_valid_name_char(*endOfPossibleFuncName))
                    {
                    std::advance(endOfPossibleFuncName, 1);
                    }
                if (*endOfPossibleFuncName == L'(' && *directiveStart != 0 &&
                    m_ctors_to_ignore.find(
                        std::wstring(directiveStart, endOfPossibleFuncName - directiveStart)) !=
                        m_ctors_to_ignore.cend())
                    {
                    directiveStart = std::next(endOfPossibleFuncName);
                    }
                // #define'd variable followed by a quote? Process as a string variable then.
                if (*directiveStart != 0 &&
                    (*directiveStart == L'\"' || (*std::next(directiveStart) == L'\"')))
                    {
                    const auto* quoteEnd =
                        string_util::find_unescaped_char(std::next(directiveStart), L'\"');
                    if (quoteEnd != nullptr && (quoteEnd - directiveStart) > 0)
                        {
                        const std::wstring definedValue = std::wstring(
                            std::next(directiveStart), (quoteEnd - directiveStart) - 1);
                        process_variable(std::wstring{}, definedTerm, definedValue,
                                         directivePos + (directiveStart - originalStart));
                        }
                    }
                // example: #define VALUE height, #define VALUE 0x5
                // No open parentheses after the defined value--then not a function.
                // Just leave the end marker where it is (EOL) and gobble all of this up.
                else if (std::wstring_view{ directiveStart,
                                            static_cast<size_t>(end - directiveStart) }
                             .find(L'(') == std::wstring_view::npos)
                    { /*no-op*/
                    }
                // ...or more like a #defined function, so let main parser deal with it
                // (just strip out the preprocessor junk here)
                else
                    {
                    end = directiveStart;
                    }
                }
            clear_section(originalStart, end);
            return end;
            }
        // unknown preprocessor, just skip the '#'
        return std::next(directiveStart);
        }
    } // namespace i18n_check
