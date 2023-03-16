/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2023
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_REVIEW_H__
#define __I18N_REVIEW_H__

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include "i18n_string_util.h"

/// @brief Classes for checking source code for internationalization/localization issues.
namespace i18n_check
    {
    /// @brief Tests to perform.
    enum review_style
        {
        /// @brief Do not perform any checks.
        no_l10n_checks = 0x00,
        /// @brief Check that strings exposed for localization are safe to translate.\n
        ///     For example, a printf statement with no actual words in it should
        ///     not be translatable.
        check_l10n_strings = 0x01,
        /// @brief Check for localizable strings being used with internal functions
        ///     (e.g., debug and logging messages).
        check_suspect_l10n_string_usage = 0x02,
        /// @brief Check for quotes strings in the source that are not available
        ///     for translation that probably should be.
        check_not_available_for_l10n = 0x04,
        /// @brief Perform all tests.
        all_l10n_checks = 
            (check_l10n_strings|check_suspect_l10n_string_usage|
             check_not_available_for_l10n)
        };

    /** @brief Class to extract and review localizable/nonlocalizable
            text from source code.*/
    class i18n_review
        {
    public:
        /// @brief Information about a string found in the source code.
        struct string_info
            {
            /// @brief How the string is being used.
            struct usage_info
                {
                /// @brief What a string is being used for
                ///     (e.g., a function parameter or being assigned to a variable).
                enum class usage_type
                    {
                    function, /*!< Message is called from within a function.*/
                    variable, /*!< Message is assigned to a variable.*/
                    orphan    /*!< Message is not connected directly to variable or function call.
                                   Instead, it is being returned or within an
                                   `if` statement (as an example).*/
                    };
                /// @private
                usage_info() = default;
                usage_info(const usage_type& type, const std::wstring& val) :
                    m_type(type), m_value(val)
                    {}
                usage_info(const usage_type& type,const std::wstring& val,
                           const std::wstring& varType) :
                    m_type(type), m_value(val), m_variableType(varType)
                    {}
                usage_type m_type{ usage_type::function };
                std::wstring m_value;
                std::wstring m_variableType;
                };
            string_info(const std::wstring& str, const usage_info& usage,
                        const std::wstring& fileName,
                        const std::pair<size_t,size_t> lineAndColumn) :
                m_string(str), m_usage(usage), m_file_name(fileName),
                m_line(lineAndColumn.first), m_column(lineAndColumn.second)
                {}
            string_info() = default;
            std::wstring m_string;
            usage_info m_usage;
            std::wstring m_file_name;
            size_t m_line{ 0 };
            size_t m_column{ 0 };
            };

        /// @brief Messages logged during a review.
        struct parse_messages
            {
            parse_messages(const std::wstring& filename, const std::wstring& str,
                           const std::string& message) :
                m_file_name(filename), m_resourceString(str), m_message(message)
                {}
            std::wstring m_file_name;
            std::wstring m_resourceString;
            std::string m_message;
            };

        /// @brief Constructor.
        i18n_review();
        /// @private
        virtual ~i18n_review() {}
        void add_function_to_ignore(const std::wstring& func)
            { m_internal_functions.insert(func); }
        void add_translation_extraction_function(const std::wstring& func)
            { m_localization_functions.insert(func); }
        void set_style(const review_style sty) noexcept
            { m_reviewStyles = sty; }
        /** @brief Main interface for extracting resource text from C++ source code.
            @param cpp_text The code text to review.
            @param text_length The length of the text.
            @param file_name The (optional) name of source file being analyzed.*/
        virtual void operator()(const wchar_t* cpp_text, const size_t text_length,
                                const std::wstring& file_name = L"") = 0;
        /// @brief Reviews any strings that are available for translation that are suspect.
        /// @note This should be called after you are finished processing all
        ///     of your files via `operator()`.
        void review_localizable_strings()
            {
            if (m_reviewStyles & check_l10n_strings)
                {
                for (const auto& i : m_localizable_strings)
                    {
                    if (i.m_string.length() &&
                        is_untranslatable_string(i.m_string, false))
                        { m_unsafe_localizable_strings.emplace_back(i); }
                    }
                }
            }
        /// @brief Reviews output integrity to see if there were any parsing errors.
        /// @note This should be called after you are finished processing all
        ///     of your files via `operator()`.
        void run_diagnostics();
        /// @returns The strings in the code that are set to be extracted as translatable. 
        [[nodiscard]]
        const std::vector<string_info>& get_localizable_strings() const noexcept
            { return m_localizable_strings; }
        /// @returns The hard-coded strings in the code that should probably
        ///     be exposed for translation. 
        [[nodiscard]]
        const std::vector<string_info>& get_not_available_for_localization_strings() const noexcept
            { return m_not_available_for_localization_strings; }
        /// @returns Strings that have been explicitly set to not be translatable
        ///     (usually by the DONTTRANSLATE() function).
        [[nodiscard]]
        const std::vector<string_info>& get_marked_as_non_localizable_strings() const noexcept
            { return m_marked_as_non_localizable_strings; }
        [[nodiscard]]
        const std::vector<string_info>& get_localizable_strings_in_internal_call() const noexcept
            { return m_localizable_strings_in_internal_call; }
        /// @returns The strings that are intentionally not translatable
        ///     (e.g., diagnostic messages).
        [[nodiscard]]
        const std::vector<string_info>& get_internal_strings() const noexcept
            { return m_internal_strings; }
        /// @returns The strings that are being extracted as localizable,
        ///     but don't appear to be safe to actually translate.
        [[nodiscard]]
        const std::vector<string_info>& get_unsafe_localizable_strings() const noexcept
            { return m_unsafe_localizable_strings; }
        /** @brief Adds a regular expression pattern to determine if a variable should be
                considered an internal string.
            @details For example, a pattern like "^utf8Name.*" would instruct
                the parser to see any string assigned to a variable starting with
                "utf8Name" to be considered some sort of internal or diagnostic message
                and that it isn't meant to be translated.

                Any variable having a string assigned to it that matches this pattern
                will be considered a string that should not be translatable.
                If a translatable string is assigned to a variable matching this
                pattern, then it will be logged as an error.
            @param pattern The regex pattern to compare against the variable names.
        */
        void add_variable_name_pattern_to_ignore(const std::wregex& pattern)
            { m_variable_name_patterns_to_ignore.emplace_back(pattern); }
        /// @returns The regex patterns compared against variables that have
        ///     strings assigned to them. @sa add_variable_name_patterns_to_ignore().
        [[nodiscard]]
        const std::vector<std::wregex>& get_ignored_variable_patterns() const noexcept
            { return m_variable_name_patterns_to_ignore; }

        /** @brief Adds a variable type to ignore.
            @details Any variables that are constructed in place with these types
                will have their string values marked as internal.
            @param varType The variable type to ignore.
            @note This only works for variables with string arguments that
                are constructed in place.*/
        void add_variable_type_to_ignore(const std::wstring& varType)
            { m_variable_types_to_ignore.insert(varType); }
        /// @returns The variable types that will have their string values marked as internal.
        const std::set<std::wstring>& get_ignored_variable_types() const noexcept
            { return m_variable_types_to_ignore; }

        /// @brief Allocates space for the results.
        /// @param fileCount The expected number of files that the parser
        ///     will be going through before.
        /// @note The results are queued up for all the files until clear_results()
        ///     is called. Therefore, this should be called prior to processing a
        ///     large number of files.
        void reserve(const size_t fileCount);
        /** @brief Clears all results from the previous parsing.
            @note This does not reset functions and variable patterns that you have the parser set
                to use; it will only reset the results from the last parsing operation.*/
        void clear_results() noexcept;
        /// @returns A list of errors (usually the regex engine having issues parsing something)
        ///     encountered while parsing the file.
        [[nodiscard]]
        const std::vector<parse_messages>& get_error_log() const noexcept
            { return m_error_log; }
        /// @returns Whether to consider punctuation-only strings as being safe to translate.
        [[nodiscard]]
        bool is_allowing_translating_punctuation_only_strings() const noexcept
            { return m_allow_translating_punctuation_only_strings; }
        /// @brief Set whether to consider punctuation-only strings as being safe to translate.
        /// @details The default is @c false.
        /// @param allow @c true to consider punctuation-only strings localizable.
        void allow_translating_punctuation_only_strings(const bool allow) noexcept
            { m_allow_translating_punctuation_only_strings = allow; }
        /// @returns Whether to verify that exception messages as available for translation.
        [[nodiscard]]
        bool should_exceptions_be_translatable() const noexcept
            { return m_exceptions_should_be_translatable; }
        /// @brief Set whether to verify that exception messages as available for translation.
        /// @details The default is @c true.
        /// @param allow @c true to consider exception messages localizable.
        void exceptions_should_be_translatable(const bool allow) noexcept
            { m_exceptions_should_be_translatable = allow; }
        /// @returns The minimum number of words that a string must have to be
        ///     reviewed for whether it should be available for translation.
        [[nodiscard]]
        size_t get_min_words_for_classifying_unavailable_string() const noexcept
            { return m_min_words_for_unavailable_string; }
        /// @brief Sets The minimum number of words that a string must have to be
        ///     reviewed for whether it should be available for translation.
        /// @details The default is to require two or more words before a string
        ///     could be considered translatable.
        /// @param minVal The word count threshold.
        void set_min_words_for_classifying_unavailable_string(const size_t minVal) noexcept
            { m_min_words_for_unavailable_string = minVal; }
        /** @brief Adds a font face to be ignored if found as a string.
            @param str The font face name.*/
        void add_font_name_to_ignore(const string_util::case_insensitive_wstring& str)
            { m_font_names.emplace(str); }
        /** @brief Adds a file extension to be ignored if found as a string.
            @param str The font face name.*/
        void add_file_extension_to_ignore(const string_util::case_insensitive_wstring& str)
            { m_file_extensions.emplace(str); }
    protected:
        /// @private
        struct exclusion_block_find_info
            {
            exclusion_block_find_info(const size_t blockStart,
                                      const size_t previousBlockEnd) noexcept :
                m_blockStart(blockStart), m_previousBlockEnd(previousBlockEnd)
                {}
            size_t m_blockStart{ 0 };
            size_t m_previousBlockEnd{ 0 };
            };
        /// Determines if a string that is marked as non-localizable should actually be
        /// exposed for translation or not. If so, then it will be added to the queue of
        /// non-localizable strings; otherwise, it will be considered an internal string.
        /// @param str The string to review.
        void classify_non_localizable_string(const string_info& str)
            {
            if (m_reviewStyles & check_not_available_for_l10n)
                {
                if (!should_exceptions_be_translatable() &&
                    m_exceptions.find(str.m_usage.m_value) != m_exceptions.cend())
                    { return; }
                if (is_untranslatable_string(str.m_string, true))
                    { m_internal_strings.emplace_back(str); }
                else
                    { m_not_available_for_localization_strings.emplace_back(str); }
                }
            }
        /// @returns @c true if a string is a keyword.
        /// @param str The string to review.
        [[nodiscard]]
        bool is_keyword(const std::wstring& str) const
            { return m_keywords.find(str) != m_keywords.cend(); }
        /// @returns @c true if string is a known font face name.
        /// @param str The string to review.
        [[nodiscard]]
        bool is_font_name(const string_util::case_insensitive_wstring& str) const
            { return m_font_names.find(str) != m_font_names.cend(); }
        /// @returns @c true if string is a known file extension.
        /// @param str The string to review.
        [[nodiscard]]
        bool is_file_extension(const string_util::case_insensitive_wstring& str) const
            { return m_file_extensions.find(str) != m_file_extensions.cend(); }

        void log_message(const std::wstring& info, const std::string& message) const
            { m_error_log.emplace_back(parse_messages(m_file_name, info, message)); }
#ifdef __UNITTEST
    public:
#endif
        /// @returns Whether @c str is a string that should be translated.
        /// @param str The string to review.
        /// @param limitWordCount If @c true, will consider a word as
        ///    untranslatable if it doesn't meet
        ///    get_min_words_for_classifying_unavailable_string()'s threshold.
        /// @note @c limitWordCount should be false if reviewing a word that is available
        ///     for l10n as these strings should always be reviewed, regardless of length.
        [[nodiscard]]
        bool is_untranslatable_string(std::wstring str, const bool limitWordCount) const;
        /// @returns Whether @c functionName is a diagnostic function (e.g., ASSERT) whose
        ///     string parameters shouldn't be translatable.
        /// @param functionName The name of the function to review.
        [[nodiscard]]
        bool is_diagnostic_function(const std::wstring& functionName) const;
        /// @returns Whether @c wc is an allowable character for function/variable names.
        /// @param wc The character to review.
        /// @note this will only work for the simple part of a function.
        ///     If you need to include namespace accessors and template information,
        ///     then use is_valid_name_char_ex() instead.
        [[nodiscard]]
        bool is_valid_name_char(const wchar_t wc) const noexcept
            {
            return (i18n_string_util::is_numeric(wc) ||
                    i18n_string_util::is_alpha_7bit(wc) ||
                    wc == L'_');
            }
        /// @returns Whether @c wc is an allowable character for function/variable names.
        /// @param wc The character to review.
        /// @note This can include extended parts of a variable/function name to include
        ///     namespace accessor and templates information. For example, this will see
        ///     `std::sort<CString>` as the full name of the function (instead of just sort).
        [[nodiscard]]
        bool is_valid_name_char_ex(const wchar_t wc) const noexcept
            {
            return (i18n_string_util::is_numeric(wc) ||
                    i18n_string_util::is_alpha_7bit(wc) ||
                    wc == L'_' ||
                    wc == L'.' || 
                    wc == L':' || 
                    wc == L'<' || 
                    wc == L'>');
            }

        /// @brief Strips off decorations from variable and functions.
        /// @details This is language specific and should be reimplemented in derived classes.
        /// @param str The string to strip.
        virtual void remove_decorations([[maybe_unused]] std::wstring& str) const
            {}

        /// @brief Reviews and classifies a string value based on the
        ///     variable it is being assigned to.
        /// @param variableType The type of the variable being assigned to
        ///     (e.g.,  wxString).
        /// @param variableName The name of the variable being assigned to.
        /// @param value The string value being reviewed.
        /// @param quotePosition The character position of where the
        ///     quote starting @c value begins.
        void process_variable(const std::wstring& variableType, const std::wstring& variableName,
                              const std::wstring& value,
                              const size_t quotePosition);

        /// @brief Fills a block with blanks.
        /// @details Useful for excluding an already processed text block.
        /// @param start The starting position.
        /// @param end The ending position.
        void clear_section(wchar_t* start, const wchar_t* end) const noexcept
            {
            for (ptrdiff_t i = 0; i < end-start; ++i)
                {
                // preserve newlines so that line position calculations work later
                start[i] = (start[i] == L'\r' || start[i] == L'\n') ? start[i] : L' ';
                }
            }

        /** @brief Backtracks from a quote to see which function or variable it is
                connected to.
            @param startPos The start of the quote.
            @param startSentinel The furthest point to look backwards.
            @param[out] functionName If the string is in a function call,
                the name of that function.
            @param[out] variableName If the string is being assigned to a variable,
                the name of that variable.
            @param[out] variableType What the string was being assigned to (function or variable).
            @returns The position of the function or variable related to the string.*/
        const wchar_t* read_var_or_function_name(const wchar_t* startPos,
            const wchar_t* const startSentinel,
            std::wstring& functionName, std::wstring& variableName, std::wstring& variableType);

        /// @returns The line and column postion from a character position.
        /// @param position The character position in the file.
        [[nodiscard]]
        std::pair<size_t, size_t> get_line_and_column(size_t position) noexcept;

        const wchar_t* m_file_start{ nullptr };

        bool m_allow_translating_punctuation_only_strings{ false };
        bool m_exceptions_should_be_translatable{ true };
        size_t m_min_words_for_unavailable_string{ 2 };

        review_style m_reviewStyles{ review_style::all_l10n_checks };

        // once these are set (by our CTOR and/or by client), they shouldn't be reset
        std::set<std::wstring> m_localization_functions;
        std::set<std::wstring> m_non_localizable_functions;
        std::set<std::wstring> m_internal_functions;
        std::set<std::wstring> m_exceptions;
        std::set<std::wstring> m_ctors_to_ignore;
        std::vector<std::wregex> m_variable_name_patterns_to_ignore;
        std::set<std::wstring> m_variable_types_to_ignore;
        std::set<string_util::case_insensitive_wstring> m_known_internal_strings;
        std::set<std::wstring> m_keywords;
        std::set<string_util::case_insensitive_wstring> m_font_names;
        std::set<string_util::case_insensitive_wstring> m_file_extensions;

        // results after parsing that the client should maybe review
        std::vector<string_info> m_localizable_strings;
        std::vector<string_info> m_marked_as_non_localizable_strings;
        std::vector<string_info> m_internal_strings;
        // results that are probably issues
        std::vector<string_info> m_unsafe_localizable_strings;
        std::vector<string_info> m_localizable_strings_in_internal_call;
        std::vector<string_info> m_not_available_for_localization_strings;

        std::wstring m_file_name;

        // regex
        std::wregex m_html_regex;
        std::wregex m_html_element_regex;
        std::wregex m_html_tag_regex;
        std::wregex m_html_tag_unicode_regex;
        std::wregex m_2letter_regex;
        std::wregex m_hashtag_regex;
        std::wregex m_key_shortcut_regex;
        std::wregex m_function_signature_regex;
        std::wregex m_plural_regex;
        std::wregex m_open_function_signature_regex;
        std::wregex m_assert_regex;
        std::vector<std::wregex> m_untranslatable_regexes;

        // helpers
        mutable std::vector<parse_messages> m_error_log;

        // bookkeeping diagnostics
    #ifndef NDEBUG
        mutable std::pair<std::wstring, std::wregex> m_longest_internal_string;
    #endif
        };
    }

/** @}*/

#endif //__I18N_REVIEW_H__
