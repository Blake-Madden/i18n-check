/** @addtogroup Utilities
    @brief Utility classes.
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

/// Classes for checking source code for internationalization/localization issues.
namespace i18n_check
    {
    /** @brief Class to extract and review localizable/nonlocalizable text from C++ source code.*/
    class i18n_review
        {
    public:
        /// Information about a string found in the source code.
        struct string_info
            {
            /// How the string is being used.
            struct usage_info
                {
                /// @brief What a string is being used for (e.g., a function parameter or being assigned to a variable).
                enum class usage_type
                    {
                    function, /*!< Message is called from within a function.*/
                    variable, /*!< Message is assigned to a variable.*/
                    orphan    /*!< Message is not connected directly to variable or function call. Instead, it is being returned or within an `if` statement (as an example).*/
                    };
                usage_info() noexcept : m_type(usage_type::function) {};
                usage_info(const usage_type& type, const std::wstring& val) : m_type(type), m_value(val) {}
                usage_info(const usage_type& type,const std::wstring& val, const std::wstring& varType) :
                    m_type(type), m_value(val), m_variableType(varType) {}
                usage_type m_type{ usage_type::function };
                std::wstring m_value;
                std::wstring m_variableType;
                };
            string_info(const std::wstring& str, const usage_info& usage, const std::wstring& fileName,
                const std::pair<size_t,size_t> lineAndColumn) :
                m_string(str), m_usage(usage), m_file_name(fileName), m_line(lineAndColumn.first), m_column(lineAndColumn.second) {}
            string_info() noexcept : m_line(0), m_column(0) {}
            std::wstring m_string;
            usage_info m_usage;
            std::wstring m_file_name;
            size_t m_line{ 0 };
            size_t m_column{ 0 };
            };

        // Messages logged during a review.
        struct parse_messages
            {
            parse_messages(const std::wstring& filename, const std::wstring& str, const std::string& message) :
                m_file_name(filename), m_resourceString(str), m_message(message) {}
            std::wstring m_file_name;
            std::wstring m_resourceString;
            std::string m_message;
            };

        // CTOR.
        i18n_review();
        virtual ~i18n_review() {}
        void add_function_to_ignore(const std::wstring& func)
            { m_internal_functions.insert(func); }
        void add_translation_extraction_function(const std::wstring& func)
            { m_localization_functions.insert(func); }
        /** @brief Main interface for extracting resource text from C++ source code.
            @param cpp_text The code text to review.
            @param text_length The length of the text.
            @param file_name The (optional) name of source file being analyzed.*/
        virtual void operator()(const wchar_t* cpp_text, const size_t text_length, const std::wstring& file_name = L"") = 0;
        /// Reviews any strings that are available for translation that are suspect.
        /// @note This should be called after you are finished processing all
        ///  of your files via operator().
        void review_localizable_strings()
            {
            for (const auto& i : m_localizable_strings)
                {
                if (i.m_string.length() && is_untranslatable_string(i.m_string))
                    { m_unsafe_localizable_strings.emplace_back(i); }
                }
            }
        /// Reviews output integrity to see if there were any parsing errors.
        /// @note This should be called after you are finished processing all
        /// of your files via operator().
        void run_diagnostics();
        /// @returns The strings in the code that are set to be extracted as translatable. 
        [[nodiscard]] const std::vector<string_info>& get_localizable_strings() const noexcept
            { return m_localizable_strings; }
        /// @returns The hard-coded strings in the code that should probably be exposed for translation. 
        [[nodiscard]] const std::vector<string_info>& get_not_available_for_localization_strings() const noexcept
            { return m_not_available_for_localization_strings; }
        /// @returns Strings that have been explicitly set to not be translatable (usually by the DONTTRANSLATE() function).
        [[nodiscard]] const std::vector<string_info>& get_marked_as_non_localizable_strings() const noexcept
            { return m_marked_as_non_localizable_strings; }
        [[nodiscard]] const std::vector<string_info>& get_localizable_strings_in_internal_call() const noexcept
            { return m_localizable_strings_in_internal_call; }
        /// @returns The strings that are intentionally not translatable (e.g., diagnostic messages).
        [[nodiscard]] const std::vector<string_info>& get_internal_strings() const noexcept
            { return m_internal_strings; }
        /// @returns The strings that are being extracted as localizable, but don't appear to be safe to actually translate.
        [[nodiscard]] const std::vector<string_info>& get_unsafe_localizable_strings() const noexcept
            { return m_unsafe_localizable_strings; }
        /** Adds a regular expression pattern to determine if a variable should be considered an internal string.
            For example, a pattern like "^utf8Name.*" would instruct
            the parser to see any string assigned to a variable starting with "utf8Name" to be considered some sort
            of internal or diagnostic message and that it isn't meant to be translated.

            Any variable having a string assigned to it that matches this pattern will be considered a string
            that should not be translatable. If a translatable string is assigned to a variable matching this
            pattern, then it will be logged as an error.
            @param pattern The regex pattern to compare against the variable names.
        */
        void add_variable_pattern_to_ignore(const std::wregex& pattern)
            { m_variable_patterns_to_ignore.emplace_back(pattern); }
        /// @returns The regex patterns compared against variables that have strings assigned to them. @sa add_variable_pattern_to_ignore().
        [[nodiscard]] const std::vector<std::wregex>& get_ignored_variable_patterns() const noexcept
            { return m_variable_patterns_to_ignore; }

        /** Adds a variable type to ignore. Any variables that are constructed in place with these types
            will have their string values marked as internal.
            @param varType The variable type to ignore.
            @note This only works for variables with string arguments that are constructed in place.*/
        void add_untranslatable_variable_type(const std::wstring& varType)
            { m_untranslatable_variable_types.insert(varType); }
        /// @returns The variable types that will have their string values marked as internal.
        const std::set<std::wstring>& get_untranslatable_variable_types() const noexcept
            { return m_untranslatable_variable_types; }

        /// Allocates space for the results.
        /// @param fileCount The expected number of files that the parser will be going through before.
        /// @note The results are queued up for all the files until clear_results() is called. Therefore,
        ///  this should be called prior to processing a large number of files.
        void reserve(const size_t fileCount);
        /** @brief Clears all results from the previous parsing.
            @note This does not reset functions and variable patterns that you have the parser set
             to use; it will only reset the results from the last parsing operation.*/
        void clear_results() noexcept;
        /// @returns A list of errors (usually the regex engine having issues parsing something) encountered while parsing the file.
        [[nodiscard]] const std::vector<parse_messages>& get_error_log() const noexcept
            { return m_error_log; }
        /// @returns Whether it is OK to consider punctuation only strings as being safe to translate.
        [[nodiscard]] bool is_allowing_translating_punctuation_only_strings() const noexcept
            { return m_allow_translating_punctuation_only_strings; }
        /// @brief Set whether it is OK to consider punctuation only strings as being safe to translate.
        void allow_translating_punctuation_only_strings(const bool allow) noexcept
            { m_allow_translating_punctuation_only_strings = allow; }
        /** @brief Adds a font face to be ignored if found as a string.
            @param str The font face name.*/
        void add_font_name_to_ignore(const string_util::case_insensitive_wstring& str)
            { m_font_names.emplace(str); }
        /** @brief Adds a file extension to be ignored if found as a string.
            @param str The font face name.*/
        void add_file_extension_to_ignore(const string_util::case_insensitive_wstring& str)
            { m_file_extensions.emplace(str); }
    protected:
        struct exclusion_block_find_info
            {
            exclusion_block_find_info(const size_t blockStart, const size_t previousBlockEnd) noexcept :
                m_blockStart(blockStart), m_previousBlockEnd(previousBlockEnd) {}
            size_t m_blockStart{ 0 };
            size_t m_previousBlockEnd{ 0 };
            };
        /// Determines if a string that is marked as non-localizable should actually be
        /// exposed for translation or not. If so, then it will be added to the queue of
        /// non-localizable strings; otherwise, it will be considered an internal string.
        void classify_non_localizable_string(const string_info& str)
            {
            if (is_untranslatable_string(str.m_string))
                { m_internal_strings.emplace_back(str); }
            else
                { m_not_available_for_localization_strings.emplace_back(str); }
            }
        [[nodiscard]] bool is_keyword(const std::wstring& str) const
            { return m_keywords.find(str) != m_keywords.cend(); }
        /// @returns True if string is a known fontface name.
        /// @param str The string to review.
        [[nodiscard]] bool is_font_name(const string_util::case_insensitive_wstring& str) const
            { return m_font_names.find(str) != m_font_names.cend(); }
        /// @returns True if string is a known file extension.
        /// @param str The string to review.
        [[nodiscard]] bool is_file_extension(const string_util::case_insensitive_wstring& str) const
            { return m_file_extensions.find(str) != m_file_extensions.cend(); }

        void log_message(const std::wstring& info, const std::string& message) const
            { m_error_log.emplace_back(parse_messages(m_file_name, info, message)); }
#ifdef __UNITTEST
    public:
#endif
        /// @returns Whether @c str is a string that should be translated.
        [[nodiscard]] bool is_untranslatable_string(std::wstring str) const;
        /// @returns Whether @c functionName is a diagnostic function (e.g., ASSERT) whose
        /// string parameters shouldn't be translatable.
        [[nodiscard]] bool is_diagnostic_function(const std::wstring& functionName) const;
        /// @returns Whether @c wc is an allowable character for function/variable names.
        /// @note this will only work for the simple part of a function. If you need to include
        /// namespace accessors and template information, then use is_valid_name_char_ex() instead.
        [[nodiscard]] bool is_valid_name_char(const wchar_t wc) const noexcept
            {
            return (i18n_string_util::is_numeric(wc) ||
                    i18n_string_util::is_alpha_7bit(wc) ||
                    wc == L'_');
            }
        /// @returns Whether @c wc is an allowable character for function/variable names.
        /// @note This can include extended parts of a variable/function name to include
        /// namespace accessor and templates information. For example, this will see
        /// `std::sort<CString>` as the full name of the function (instead of just sort).
        [[nodiscard]] bool is_valid_name_char_ex(const wchar_t wc) const noexcept
            {
            return (i18n_string_util::is_numeric(wc) ||
                    i18n_string_util::is_alpha_7bit(wc) ||
                    wc == L'_' ||
                    wc == L'.' || 
                    wc == L':' || 
                    wc == L'<' || 
                    wc == L'>');
            }

        /// Strips off decorations from variable and functions. This is language specific and should
        /// be reimplemented in derived classes.
        virtual void remove_decorations([[maybe_unused]] std::wstring& str) const
            {}

        /// Reviews and classifies a string value based on its variable name.
        void process_variable(const std::wstring& variableType, const std::wstring& variableName,
                              const std::wstring& value,
                              const size_t quotePosition);

        /// Fills a block with blanks. Useful for excluding an already processed text block.
        void clear_section(wchar_t* start, const wchar_t* end) const noexcept
            {
            for (ptrdiff_t i = 0; i < end-start; ++i)
                {
                // preserve newlines so that line position calculations work later
                start[i] = (start[i] == L'\r' || start[i] == L'\n') ? start[i] : L' ';
                }
            }

        const wchar_t* read_var_or_function_name(const wchar_t* startPos, const wchar_t* const startSentinel,
            std::wstring& functionName, std::wstring& variableName, std::wstring& variableType);

        std::pair<size_t, size_t> get_line_and_column(size_t position) noexcept;

        const wchar_t* m_file_start{ nullptr };

        bool m_allow_translating_punctuation_only_strings{ false };

        // once these are set (by our CTOR and/or by client), they shouldn't be reset
        std::set<std::wstring> m_localization_functions;
        std::set<std::wstring> m_non_localizable_functions;
        std::set<std::wstring> m_internal_functions;
        std::set<std::wstring> m_ctors_to_ignore;
        std::vector<std::wregex> m_variable_patterns_to_ignore;
        std::set<std::wstring> m_untranslatable_variable_types;
        std::set<string_util::case_insensitive_wstring> m_known_internal_strings;
        std::set<std::wstring> m_keywords;
        std::set<string_util::case_insensitive_wstring> m_font_names;
        std::set<string_util::case_insensitive_wstring> m_file_extensions;

        // results after parsing that the client should maybe review
        std::vector<string_info> m_localizable_strings;
        std::vector<string_info> m_marked_as_non_localizable_strings;
        std::vector<string_info> m_internal_strings;
        // results that are probably issues
        std::vector<string_info> m_not_available_for_localization_strings;
        std::vector<string_info> m_unsafe_localizable_strings;
        std::vector<string_info> m_localizable_strings_in_internal_call;

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
