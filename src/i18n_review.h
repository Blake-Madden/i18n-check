/********************************************************************************
 * Copyright (c) 2021-2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef __I18N_REVIEW_H__
#define __I18N_REVIEW_H__

#include "donttranslate.h"
#include "i18n_string_util.h"
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>
#ifndef CPPCHECK_SKIP_SECTION
    // for the GUI version, include gettext's translation loading support via wxWidgets
    #if __has_include(<wx/wx.h>)
        #include <wx/wx.h>
        #if wxCHECK_VERSION(3, 3, 0)
            #define _WXTRANS_WSTR(s) _(s).wc_string()
        #else
            #define _WXTRANS_WSTR(s) _(s).wc_str()
        #endif
    #else
        #define _(s) (s)
        #define _WXTRANS_WSTR(s) (s)
    #endif
#endif

///@brief Checks which version of GCC we are building with.
///     (And actually GCC, not clang.) Always returns @c true if not GCC.
#if defined(__clang__)
    #define CHECK_GCC_VERSION(major, minor, patch) 1
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
    #define CHECK_GCC_VERSION(major, minor, patch)                                                 \
        ((__GNUC__ > (major)) || (__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) ||              \
         (__GNUC__ == (major) && __GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patch)))
#else
    // if not compiling with GCC, then treat anything as true
    #define CHECK_GCC_VERSION(major, minor, patch) 1
#endif

/// @brief Classes for checking source code for internationalization/localization issues.
/// @details Refer to https://www.gnu.org/software/gettext/manual/gettext.html
///     for i18n best practices, which this library attempts to enforce.\n
///     https://techbase.kde.org/Development/Tutorials/Localization/i18n_Mistakes is a good
///     reference for i18n pitfalls.
/// @par References:
///     https://www.gnu.org/software/gettext/manual/gettext.html <br />
///     https://www.gnu.org/software/gettext/ <br />
///     https://docs.wxwidgets.org/latest/classwx_translations.html <br />
///     https://doc.qt.io/qt-5/i18n-source-translation.html <br />
///     https://develop.kde.org/docs/plasma/widget/translations-i18n/ <br />
///     https://learn.microsoft.com/en-us/cpp/mfc/tn020-id-naming-and-numbering-conventions?view=msvc-170
///     <br /> https://learn.microsoft.com/en-us/windows/win32/menurc/stringtable-resource
namespace i18n_check
    {
    /// @brief Tests to perform.
    enum review_style : int64_t
        {
        /// @brief Do not perform any checks.
        no_checks = 0x00,
        /// @brief Check that strings exposed for localization are safe to translate.\n
        ///     For example, a printf statement with no actual words in it should
        ///     not be translatable.
        check_l10n_strings = (static_cast<int64_t>(1) << 0),
        /// @brief Check for localizable strings being used with internal functions
        ///     (e.g., debug and logging messages).
        check_suspect_l10n_string_usage = (static_cast<int64_t>(1) << 1),
        /// @brief Check for quotes strings in the source that are not available
        ///     for translation that probably should be.
        check_not_available_for_l10n = (static_cast<int64_t>(1) << 2),
        /// @brief Check for deprecated text macros (e.g., `wxT()`).
        check_deprecated_macros = (static_cast<int64_t>(1) << 3),
        /// @brief Check that files containing extended ASCII characters are UTF-8 encoded.
        check_utf8_encoded = (static_cast<int64_t>(1) << 4),
        /// @brief Check for strings that contain extended ASCII characters that are not encoded.
        check_unencoded_ext_ascii = (static_cast<int64_t>(1) << 5),
        /// @brief Check for printf() commands being used to format a single number
        ///     (without any particular precision or padding features).\n
        ///     In this situation, it is better to use std::to_string() to avoid
        ///     potentially dangerous printf() commands.
        check_printf_single_number = (static_cast<int64_t>(1) << 6),
        /// @brief Check for URLs or email addresses inside translatable strings.
        /// @details It is recommended to format those into the strings dynamically,
        ///     so that translators do not have to manage them.
        check_l10n_contains_url = (static_cast<int64_t>(1) << 7),
        /// @brief Check for ID variables being assigned a hard-coded number.
        /// @details It may be preferred to assign framework-defined constant to ID.
        check_number_assigned_to_id = (static_cast<int64_t>(1) << 8),
        /// @brief Check for the same value being assigned to different ID variables.
        check_duplicate_value_assigned_to_ids = (1 << 9),
        /// @brief Check for malformed syntax in strings (e.g., malformed HTML tags).
        check_malformed_strings = (static_cast<int64_t>(1) << 10),
        /// @brief Check for UTF-8 encoded files which start with a BOM/UTF-8 signature.
        check_utf8_with_signature = (static_cast<int64_t>(1) << 11),
        /// @brief Check for font issues (e.g., Windows *.RC dialogs not using MS Shell Dlg
        ///     or using unusual font sizes).
        check_fonts = (static_cast<int64_t>(1) << 12),
        /// @brief Check for localizable strings that begin or end with a space.
        /// @details This may indicate that the string is being concatenated at runtime with
        ///     another value, rather than being format via a `printf` function.
        /// @note This only checks for the space character, not tabs or newlines.
        check_l10n_has_surrounding_spaces = (static_cast<int64_t>(1) << 13),
        /// @private
        i18n_reserved2 = (static_cast<int64_t>(1) << 14),
        /// @private
        i18n_reserved3 = (static_cast<int64_t>(1) << 15),
        /// @private
        i18n_reserved4 = (static_cast<int64_t>(1) << 16),
        /// @private
        i18n_reserved5 = (static_cast<int64_t>(1) << 17),
        /// @private
        i18n_reserved6 = (static_cast<int64_t>(1) << 18),
        /// @private
        i18n_reserved7 = (static_cast<int64_t>(1) << 19),
        /// @private
        i18n_reserved8 = (static_cast<int64_t>(1) << 20),
        /// @private
        i18n_reserved9 = (static_cast<int64_t>(1) << 21),
        /// @private
        i18n_reserved10 = (static_cast<int64_t>(1) << 22),
        /// @private
        i18n_reserved11 = (static_cast<int64_t>(1) << 23),
        /// @private
        i18n_reserved12 = (static_cast<int64_t>(1) << 24),
        /// @private
        i18n_reserved13 = (static_cast<int64_t>(1) << 25),
        /// @private
        i18n_reserved14 = (static_cast<int64_t>(1) << 26),
        /// @private
        i18n_reserved15 = (static_cast<int64_t>(1) << 27),
        /// @private
        i18n_reserved16 = (static_cast<int64_t>(1) << 28),
        /// @private
        i18n_reserved17 = (static_cast<int64_t>(1) << 29),
        /// @brief Perform all aforementioned internationalization checks.
        all_i18n_checks = (check_l10n_strings | check_suspect_l10n_string_usage |
        check_not_available_for_l10n | check_deprecated_macros | check_utf8_encoded |
        check_unencoded_ext_ascii | check_printf_single_number | check_l10n_contains_url |
        check_number_assigned_to_id | check_duplicate_value_assigned_to_ids |
        check_malformed_strings | check_utf8_with_signature | check_fonts),

        /// @brief Check for mismatching printf commands between source strings and their
        ///     respective translations.
        check_mismatching_printf_commands = (static_cast<int64_t>(1) << 30),
        /// @brief Check mismatching keyboard accelerators between the source and target strings.
        check_accelerators = (static_cast<int64_t>(1) << 31),
        /// @private
        l10n_reserved2 = (static_cast<int64_t>(1) << 32),
        /// @private
        l10n_reserved3 = (static_cast<int64_t>(1) << 33),
        /// @private
        l10n_reserved4 = (static_cast<int64_t>(1) << 34),
        l10n_reserved5 = (static_cast<int64_t>(1) << 35),
        l10n_reserved6 = (static_cast<int64_t>(1) << 36),
        l10n_reserved7 = (static_cast<int64_t>(1) << 37),
        l10n_reserved8 = (static_cast<int64_t>(1) << 38),
        l10n_reserved9 = (static_cast<int64_t>(1) << 39),
        l10n_reserved10 = (static_cast<int64_t>(1) << 40),
        l10n_reserved11 = (static_cast<int64_t>(1) << 41),
        l10n_reserved12 = (static_cast<int64_t>(1) << 42),
        l10n_reserved13 = (static_cast<int64_t>(1) << 43),
        l10n_reserved14 = (static_cast<int64_t>(1) << 44),
        l10n_reserved15 = (static_cast<int64_t>(1) << 45),
        l10n_reserved16 = (static_cast<int64_t>(1) << 46),
        l10n_reserved17 = (static_cast<int64_t>(1) << 47),
        l10n_reserved18 = (static_cast<int64_t>(1) << 48),
        l10n_reserved19 = (static_cast<int64_t>(1) << 49),
        /// @brief Perform all aforementioned localization checks.
        all_l10n_checks = (check_mismatching_printf_commands | check_accelerators),

        /// @brief Check for trailing spaces at the end of each line.
        check_trailing_spaces = (static_cast<int64_t>(1) << 50),
        /// @brief Check for tabs (spaces are recommended).
        check_tabs = (static_cast<int64_t>(1) << 51),
        /// @brief Check for overly long lines.
        check_line_width = (static_cast<int64_t>(1) << 52),
        /// @brief Check that there is a space at the start of a comment.
        check_space_after_comment = (static_cast<int64_t>(1) << 53),
        /// @brief Check all aforementioned code formatting issues.
        all_code_formatting_checks =
        (check_trailing_spaces | check_tabs | check_line_width | check_space_after_comment)
        };

    /// @brief Types of translation (i.e., l10n) issues.
    enum class translation_issue
        {
        /// @brief Inconsistent printf format specifiers.
        printf_issue,
        /// @brief Suspect string exposed for translation in the catalog.
        suspect_source_issue,
        /// @brief Inconsistent keyboard accelerators.
        accelerator_issue
        };

    /// @brief File types that can be analyzed.
    enum class file_review_type
        {
        /// @brief C/C++ source files.
        cpp,
        /// @brief Microsoft Windows resource files.
        rc,
        /// @brief GNU gettext catalog files.
        po,
        /// @brief C# source files.
        cs
        };

    /// @brief Types of printf languages that a PO file can contain.
    /// @details This determines which type of syntax is expected.
    enum class po_format_string
        {
        /// @brief This string is not meant for `printf()`-like functions.
        no_format,
        /// @brief C/C++ style `printf()`.
        cpp_format,
        /// @brief Objective-C.
        objc_format
        };

    /// @brief An entry in a PO file, which contains source and translation strings,
    ///     printf syntax that they use, and any encountered issues.
    struct translation_catalog_entry
        {
        /// @brief The main source string.
        std::wstring m_source;
        /// @brief An optional plural variation of the source string.
        std::wstring m_source_plural;
        /// @brief The translation of the main source string.
        std::wstring m_translation;
        /// @brief The translation of the optional plural form of the source string.
        std::wstring m_translation_plural;
        /// @brief The printf syntax used by the strings.\n
        ///     Only used for gettext PO files.
        po_format_string m_po_format{ po_format_string::no_format };
        /// @brief Any issues detected in this entry.
        std::vector<std::pair<translation_issue, std::wstring>> m_issues;
        /// @brief The position in the file.
        size_t m_line{ std::wstring_view::npos };
        };

    /// @brief Progress callback for analyze().
    /// @details This passes back the current counter and name of the file
    /// currently being analyzed.
    using analyze_callback = std::function<bool(const size_t, const std::filesystem::path&)>;
    /// @brief Can reset the progress mechanism that the associated analyze_callback is using.
    /// @details This passes back the number of items that the progress callback should expect.
    using analyze_callback_reset = std::function<void(const size_t)>;

    /** @brief Class to extract and review localizable/nonlocalizable
            text from source code.*/
    class i18n_review
        {
      public:
        /// @brief Information about a string found in the source code.
        struct string_info
            {
            /// @brief What the string is being used for.
            struct usage_info
                {
                /// @brief What a string is being used for
                ///     (e.g., a function parameter or being assigned to a variable).
                enum class usage_type
                    {
                    function, /*!< Message is called from within a function.*/
                    variable, /*!< Message is assigned to a variable.*/
                    orphan    /*!< Message is not connected directly to variable or function call.
                                   Instead, it is being returned, being used in a comparison,
                                   or within an `if` statement (as a few examples).*/
                    };
                /// @private
                usage_info() = default;

                /// @private
                usage_info(const usage_type& type, std::wstring val, std::wstring varType)
                    : m_type(type), m_value(std::move(val)), m_variableType(std::move(varType))
                    {
                    }

                /// @private
                explicit usage_info(std::wstring val) : m_value(std::move(val)) {}

                /// @private
                usage_type m_type{ usage_type::function };
                /// @private
                std::wstring m_value;
                /// @private
                std::wstring m_variableType;
                };

            /** @brief Constructor.
                @param str The string value.
                @param usage What the string is being used for.
                @param fileName The filename.
                @param lineAndColumn The line and column number.*/
            string_info(std::wstring str, usage_info usage, std::filesystem::path fileName,
                        const std::pair<size_t, size_t> lineAndColumn)
                : m_string(std::move(str)), m_usage(std::move(usage)),
                  m_file_name(std::move(fileName)), m_line(lineAndColumn.first),
                  m_column(lineAndColumn.second)
                {
                }

            /// @private
            string_info() = default;
            /// @brief The string value.
            std::wstring m_string;
            /// @brief What the string is being used for.
            usage_info m_usage;
            /// @brief The filename.
            std::filesystem::path m_file_name;
            /// @brief The line number.
            size_t m_line{ 0 };
            /// @brief The column number.
            size_t m_column{ 0 };
            };

        /// @brief Messages logged during a review.
        struct parse_messages
            {
            /** @brief Constructor.
                @param filename The filepath.
                @param positionInFile The line and column position in the file.
                @param str The string resource.
                @param message Diagnostic message.*/
            parse_messages(std::filesystem::path filename,
                           const std::pair<size_t, size_t> positionInFile, std::wstring str,
                           std::wstring message)
                : m_file_name(std::move(filename)), m_resourceString(std::move(str)),
                  m_message(std::move(message)), m_line(positionInFile.first),
                  m_column(positionInFile.second)
                {
                }

            /// @brief The filepath.
            std::filesystem::path m_file_name;
            /// @brief The string resource.
            std::wstring m_resourceString;
            /// @brief Diagnostic message.
            std::wstring m_message;
            /// @brief Line position in the file.
            size_t m_line{ std::wstring::npos };
            /// @brief Column position in the file.
            size_t m_column{ std::wstring::npos };
            };

        /// @brief Constructor.
        i18n_review();

        /// @private
        virtual ~i18n_review() {}

        /** @brief Adds a function to be considered as internal (e.g., debug functions).
            @details Strings passed to this function will not be considered translatable.
            @param func The function to ignore.*/
        void add_function_to_ignore(const std::wstring& func) { m_internal_functions.insert(func); }

        /** @brief Adds a function that loads hard-coded strings as translatable resources.
            @details This is usually functions like those found in the gettext library.
            @param func The function to consider as a resource-loading function.*/
        void add_translation_extraction_function(const std::wstring& func)
            {
            m_localization_functions.insert(func);
            }

        /** @brief Sets which checks are being performed.
            @param sty The checks to perform.*/
        void set_style(const review_style sty) noexcept { m_reviewStyles = sty; }

        /// @returns The checks being performed.
        [[nodiscard]]
        review_style get_style() const noexcept
            {
            return m_reviewStyles;
            }

        /** @brief Main interface for extracting resource text from C++ source code.
            @param file_text The text to review.
            @param file_name The (optional) name of source file being analyzed.*/
        virtual void
        operator()(std::wstring_view file_text,
                   const std::filesystem::path& file_name = std::filesystem::path{}) = 0;

        /** @brief Finalizes the review process after all files have been loaded.
            @details Reviews any strings that are available for translation that are suspect,
                cleans the strings, and logs any parsing issues.
            @param resetCallback Callback function to tell the progress system in @c callback
                how many items to expect to be processed.
            @param callback Callback function to display the progress.
                Takes the current file index, overall file count, and the name of the current file.
                Returning @c false indicates that the user cancelled the analysis.
            @note This should be called after you are finished processing all
                of your files via `operator()`.*/
        virtual void review_strings([[maybe_unused]] analyze_callback_reset resetCallback,
                                    [[maybe_unused]] analyze_callback callback);

        /// @returns The strings in the code that are set to be extracted as translatable.
        [[nodiscard]]
        const std::vector<string_info>& get_localizable_strings() const noexcept
            {
            return m_localizable_strings;
            }

        /// @returns The hard-coded strings in the code that should probably
        ///     be exposed for translation.
        [[nodiscard]]
        const std::vector<string_info>& get_not_available_for_localization_strings() const noexcept
            {
            return m_not_available_for_localization_strings;
            }

        /// @returns The deprecated text macros that were encountered.
        [[nodiscard]]
        const std::vector<string_info>& get_deprecated_macros() const noexcept
            {
            return m_deprecated_macros;
            }

        /// @returns Strings that have been explicitly set to not be translatable
        ///     (usually by the DONTTRANSLATE() function).
        [[nodiscard]]
        const std::vector<string_info>& get_marked_as_non_localizable_strings() const noexcept
            {
            return m_marked_as_non_localizable_strings;
            }

        [[nodiscard]]
        const std::vector<string_info>& get_localizable_strings_in_internal_call() const noexcept
            {
            return m_localizable_strings_in_internal_call;
            }

        /// @returns The strings that are intentionally not translatable
        ///     (e.g., diagnostic messages).
        [[nodiscard]]
        const std::vector<string_info>& get_internal_strings() const noexcept
            {
            return m_internal_strings;
            }

        /// @returns The strings that are being extracted as localizable,
        ///     but don't appear to be safe to actually translate.
        [[nodiscard]]
        const std::vector<string_info>& get_unsafe_localizable_strings() const noexcept
            {
            return m_unsafe_localizable_strings;
            }

        /// @returns The strings that are being extracted as localizable,
        ///     but contain URLs or email addresses.
        [[nodiscard]]
        const std::vector<string_info>& get_localizable_strings_with_urls() const noexcept
            {
            return m_localizable_strings_with_urls;
            }

        /// @returns The strings that are being extracted as localizable,
        ///     but are surrounded by spaces.
        [[nodiscard]]
        const std::vector<string_info>&
        get_localizable_strings_with_surrounding_spaces() const noexcept
            {
            return m_localizable_strings_with_surrounding_spaces;
            }

        /// @returns The strings that contain extended ASCII characters, but are not encoded.
        [[nodiscard]]
        const std::vector<string_info>& get_unencoded_ext_ascii_strings() const noexcept
            {
            return m_unencoded_strings;
            }

        /// @returns The strings that are a printf() command that only formats one number.\n
        ///     It is simpler to use std::to_string() variants to avoid potentially dangerous
        ///     printf() calls.
        [[nodiscard]]
        const std::vector<string_info>& get_printf_single_numbers() const noexcept
            {
            return m_printf_single_numbers;
            }

        /// @returns IDs that are assigned a hard-coded numeric value.
        [[nodiscard]]
        const std::vector<string_info>& get_ids_assigned_number() const noexcept
            {
            return m_ids_assigned_number;
            }

        /// @returns IDs that have the same value assigned to them.
        [[nodiscard]]
        const std::vector<string_info>& get_duplicates_value_assigned_to_ids() const noexcept
            {
            return m_duplicates_value_assigned_to_ids;
            }

        /// @returns Malformed strings.
        [[nodiscard]]
        const std::vector<string_info>& get_malformed_strings() const noexcept
            {
            return m_malformed_strings;
            }

        /// @returns Lines with trailing spaces.
        [[nodiscard]]
        const std::vector<string_info>& get_trailing_spaces() const noexcept
            {
            return m_trailing_spaces;
            }

        /// @returns Tabs in the files.
        [[nodiscard]]
        const std::vector<string_info>& get_tabs() const noexcept
            {
            return m_tabs;
            }

        /// @returns Overly long lines.
        [[nodiscard]]
        const std::vector<string_info>& get_wide_lines() const noexcept
            {
            return m_wide_lines;
            }

        /// @returns Comments missing a space at their start.
        [[nodiscard]]
        const std::vector<string_info>& get_comments_missing_space() const noexcept
            {
            return m_comments_missing_space;
            }

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
            @param pattern The regex pattern to compare against the variable names.*/
        static void add_variable_name_pattern_to_ignore(const std::wregex& pattern)
            {
            m_variable_name_patterns_to_ignore.push_back(pattern);
            }

        /// @returns The regex patterns compared against variables that have
        ///     strings assigned to them. @sa add_variable_name_patterns_to_ignore().
        [[nodiscard]]
        static const std::vector<std::wregex>& get_ignored_variable_patterns() noexcept
            {
            return m_variable_name_patterns_to_ignore;
            }

        /** @brief Adds a variable type to ignore.
            @details Any variables that are constructed in place with these types
                will have their string values marked as internal.
            @param varType The variable type to ignore.
            @note This only works for variables with string arguments that
                are constructed in place.*/
        static void add_variable_type_to_ignore(const std::wstring& varType)
            {
            m_variable_types_to_ignore.insert(varType);
            }

        /// @returns The variable types that will have their string values marked as internal.
        [[nodiscard]]
        static const std::set<std::wstring>& get_ignored_variable_types() noexcept
            {
            return m_variable_types_to_ignore;
            }

        /// @brief Allocates space for the results.
        /// @param fileCount The expected number of files that the parser
        ///     will be going through before.
        /// @note The results are queued up for all the files until clear_results()
        ///     is called. Therefore, this should be called prior to processing a
        ///     large number of files.
        void reserve(const size_t fileCount);
        /** @brief Clears all results from the previous parsing.
            @note This does not reset functions and variable patterns that you have added
                to the parser; it will only reset the results from the last parsing operation.*/
        void clear_results() noexcept;

        /// @returns A list of errors (usually the regex engine having issues parsing something)
        ///     encountered while parsing the file.
        [[nodiscard]]
        const std::vector<parse_messages>& get_error_log() const noexcept
            {
            return m_error_log;
            }

        /// @returns Whether to consider punctuation-only strings as being safe to translate.
        [[nodiscard]]
        bool is_allowing_translating_punctuation_only_strings() const noexcept
            {
            return m_allow_translating_punctuation_only_strings;
            }

        /// @brief Set whether to consider punctuation-only strings as being safe to translate.
        /// @details The default is @c false.
        /// @param allow @c true to consider punctuation-only strings localizable.
        void allow_translating_punctuation_only_strings(const bool allow) noexcept
            {
            m_allow_translating_punctuation_only_strings = allow;
            }

        /// @returns Whether to verify that exception messages are available for translation.
        [[nodiscard]]
        bool should_exceptions_be_translatable() const noexcept
            {
            return m_exceptions_should_be_translatable;
            }

        /// @brief Set whether to verify that exception messages are available for translation.
        /// @details The default is @c true.
        /// @param allow @c true to require that exception messages are localizable.
        void exceptions_should_be_translatable(const bool allow) noexcept
            {
            m_exceptions_should_be_translatable = allow;
            }

        /// @returns Whether strings sent to logging functions can be translatable.
        [[nodiscard]]
        bool can_log_messages_be_translatable() const noexcept
            {
            return m_log_messages_are_translatable;
            }

        /// @brief Set whether strings sent to logging functions can be translatable.
        /// @details The default is @c true.
        /// @note This will not require that strings sent to logging functions
        ///     must be localizable, only that it is OK it send translatable strings
        ///     to functions. Setting this to @c false will signal
        ///     warnings when localizable strings are sent to these types of functions.
        /// @param allow @c true to consider log messages as localizable.
        void log_messages_can_be_translatable(const bool allow) noexcept
            {
            m_log_messages_are_translatable = allow;
            }

        /// @returns The minimum number of words that a string must have to be
        ///     considered translatable.
        [[nodiscard]]
        size_t get_min_words_for_classifying_unavailable_string() const noexcept
            {
            return m_min_words_for_unavailable_string;
            }

        /// @brief Sets the minimum number of words that a string must have to be
        ///     considered translatable.
        /// @details The default is to require two or more words before a string
        ///     could be considered translatable.
        /// @param minVal The word count threshold.
        void set_min_words_for_classifying_unavailable_string(const size_t minVal) noexcept
            {
            m_min_words_for_unavailable_string = minVal;
            }

        /// @brief Sets the C++ standard that should be assumed when issuing
        ///     deprecated macro warnings.
        /// @param version The C++ standard that controls deprecation warnings.
        void set_min_cpp_version(const int version) { m_min_cpp_version = version; }

        /** @brief Adds a font face to be ignored if found as a string.
            @param str The font face name.*/
        static void add_font_name_to_ignore(const string_util::case_insensitive_wstring& str)
            {
            m_font_names.insert(str);
            }

        /** @brief Adds a file extension to be ignored if found as a string.
            @param str The font face name.*/
        static void add_file_extension_to_ignore(const string_util::case_insensitive_wstring& str)
            {
            m_file_extensions.insert(str);
            }

        /// @returns @c true if string is a known font face name.
        /// @param str The string to review.
        [[nodiscard]]
        static bool is_font_name(const string_util::case_insensitive_wstring& str)
            {
            return m_font_names.find(str) != m_font_names.cend();
            }

        /// @returns @c true if string is a known file extension.
        /// @param str The string to review.
        [[nodiscard]]
        static bool is_file_extension(const string_util::case_insensitive_wstring& str)
            {
            return m_file_extensions.find(str) != m_file_extensions.cend();
            }

        /** @brief Loads all `printf` format commands from a string.
            @param resource The string to parse.
            @returns A vector of positions and lengths of all printf commands from the string.*/
        [[nodiscard]]
        static std::vector<std::pair<size_t, size_t>>
        load_cpp_printf_command_positions(const std::wstring& resource);

        /** @brief Loads all file filters (e.g., "*.docx;*.doc") from a string.
            @param resource The string to parse.
            @returns A vector of positions and lengths of all file filters from the string.*/
        [[nodiscard]]
        static std::vector<std::pair<size_t, size_t>>
        load_file_filter_positions(const std::wstring& resource);

        /** @brief Finds and returns the next translation entry in a gettext po file.
            @param poFileText the po file content to parse.
            @returns If an entry is found, returns @c true, a view of the block, and its
                starting position in @c poFileText.*/
        [[nodiscard]]
        static std::tuple<bool, std::wstring_view, size_t>
        read_po_catalog_entry(std::wstring_view& poFileText);

        /** @brief Finds and returns the message in a gettext catalog entry.
            @param poCatalogEntry the po file catalog entry to parse.
            @param msgTag The type of message to look for (e.g., msgid or msgstr).
            @returns If an entry is found, returns @c true, the message, its
                starting position in the entry section, and its length.*/
        [[nodiscard]]
        static std::tuple<bool, std::wstring, size_t, size_t>
        read_po_msg(std::wstring_view& poCatalogEntry, const std::wstring_view msgTag);

      protected:
        /// @returns @c true if the next word in @c commentBlock is a suppression command,
        ///     along with the position of the end of the suppressed block of code.
        /// @param commentBlock The comment to review (should be after the starting comment tag).
        static std::pair<bool, size_t> is_block_suppressed(std::wstring_view commentBlock);

        // traditionally, 80 chars is the recommended line width,
        // but 120 is a bit more reasonable
        constexpr static auto m_max_line_length{ 120 };

        /// @private
        struct exclusion_block_find_info
            {
            exclusion_block_find_info(const size_t blockStart,
                                      const size_t previousBlockEnd) noexcept
                : m_blockStart(blockStart), m_previousBlockEnd(previousBlockEnd)
                {
                }

            size_t m_blockStart{ 0 };
            size_t m_previousBlockEnd{ 0 };
            };

        /// @returns @c true if a string starts or ends with spaces.
        ///     (Not tabs or newlines, but actual spaces.)
        /// @param str The string to review.
        [[nodiscard]]
        static bool has_surrounding_spaces(const std::wstring& str)
            {
            if (str.starts_with(L' '))
                {
                return true;
                }
            // ending with ": " is usually OK, as this is more about formatting
            // tabular data, rather than piecing strings together
            if (str.ends_with(L' ') && !str.ends_with(L": "))
                {
                return true;
                }
            return false;
            }

        /** @brief Processes a quote after its positions and respective
                function/variable assignment has been found.
            @param[in,out] currentTextPos The current position into the text buffer.\n
                This position (up to @c quoteEnd) will be filled with spaces after
                this section is processed.
            @param quoteEnd The end of the quote.
            @param functionVarNamePos The position in the buffer of the quote's
                function or variable being assigned to.
            @param variableName If this quote is being assigned to a variable, the variable name.
            @param functionName If this quote is inside of function, the function name.
            @param variableType If being assigned to a variable, the variable's type.
            @param deprecatedMacroEncountered If the quote is inside of a deprecated
                macro, then name of this macro.
            @param parameterPosition The string's position in the function call (if applicable).*/
        void process_quote(wchar_t* currentTextPos, const wchar_t* quoteEnd,
                           const wchar_t* functionVarNamePos, const std::wstring& variableName,
                           const std::wstring& functionName, const std::wstring& variableType,
                           const std::wstring& deprecatedMacroEncountered,
                           const size_t parameterPosition);

        /// @brief Determines whether a hard-coded string should actually be
        ///     exposed for translation or not.
        /// @details If so, then it will be added to the queue of non-localizable strings;
        ///     otherwise, it will be considered an internal string.
        /// @param str The string to review.
        void classify_non_localizable_string(string_info str);

        /// @returns Just the function name from a full function call, stripping
        ///     off any class or namespace information.
        /// @param str The string to extract the function name from.
        /// @note This assumes that the functions trailing parentheses and template
        ///     specifications have already been removed.
        [[nodiscard]]
        std::wstring_view extract_base_function(const std::wstring_view str) const;

        /// @returns @c true if a function name is a translation extraction function.
        /// @param functionName The function name to review.
        [[nodiscard]]
        bool is_i18n_function(const std::wstring_view functionName) const
            {
            return m_localization_functions.find(functionName) != m_localization_functions.cend() ||
                   m_localization_functions.find(extract_base_function(functionName)) !=
                       m_localization_functions.cend();
            }

        /// @returns @c true if a function name is a translation noop function.
        /// @param functionName The function name to review.
        [[nodiscard]]
        bool is_non_i18n_function(const std::wstring_view& functionName) const
            {
            return m_non_localizable_functions.find(functionName) !=
                       m_non_localizable_functions.cend() ||
                   m_non_localizable_functions.find(extract_base_function(functionName)) !=
                       m_non_localizable_functions.cend();
            }

        /// @returns @c true if a string is a keyword.
        /// @note These are labels and commands (e.g., else, return),
        ///     not intrinsic types (e.g., float, int).
        /// @param str The string to review.
        [[nodiscard]]
        bool is_keyword(const std::wstring& str) const
            {
            return m_keywords.find(str) != m_keywords.cend();
            }

        /** @brief Loads all `printf` format commands from a string.
            @param resource The string to parse.
            @param[out] errorInfo Information about any bad printf commands\n
                (e.g., positional and regular commands in the same string).
            @returns All `printf` format commands from @c resource.
            @details `printf` syntax:

             https://en.cppreference.com/w/c/io/fprintf

             `printf` positional arguments:

             https://learn.microsoft.com/en-us/cpp/c-runtime-library/printf-p-positional-parameters?view=msvc-170*/
        [[nodiscard]]
        static std::vector<std::wstring> load_cpp_printf_commands(const std::wstring& resource,
                                                                  std::wstring& errorInfo);

        /** @brief Logs a debug message.
            @param info Information, such as a string causing a parsing error.
            @param message An informational message.
            @param positionInFile The position in the file.*/
        void log_message(const std::wstring& info, const std::wstring& message,
                         const size_t positionInFile) const
            {
            m_error_log.push_back(
                parse_messages(m_file_name, get_line_and_column(positionInFile), info, message));
            }

        /** @brief Loads ID assignments in the text to see if there are
                hard-coded numbers or duplicated assignments.
            @param fileText The source file's text to analyze.
            @param fileName The file name being analyzed.*/
        void load_id_assignments(const std::wstring_view fileText,
                                 const std::filesystem::path& fileName);
        /** @brief Loads any deprecated functions found in the text.
            @param fileText The source file's text to analyze.
            @param fileName The file name being analyzed.*/
        void load_deprecated_functions(const std::wstring_view fileText,
                                       const std::filesystem::path& fileName);
#ifdef __UNITTEST
      public:
#endif
        /// @returns Whether @c str is a string that should probably not be translated.
        /// @param strToReview The string to review.\n
        ///     This string may have text like HTML and CSS tags removed, as well as being trimmed.
        /// @param limitWordCount If @c true, will consider a word as
        ///    untranslatable if it doesn't meet
        ///    get_min_words_for_classifying_unavailable_string()'s threshold.
        /// @note @c limitWordCount should be @c false if reviewing a word that is available
        ///     for l10n as these strings should always be reviewed for safety reasons,
        ///     regardless of length.
        [[nodiscard]]
        bool is_untranslatable_string(const std::wstring& strToReview,
                                      const bool limitWordCount) const;
        /// @returns Whether @c functionName is a diagnostic function (e.g., ASSERT) whose
        ///     string parameters shouldn't be translatable.
        /// @param functionName The name of the function to review.
        [[nodiscard]]
        bool is_diagnostic_function(const std::wstring& functionName) const;

        /// @returns Whether @c wc is an allowable character for function/variable names.
        /// @param wc The character to review.
        /// @note This will only work for the simple part of a function.\n
        ///     If you need to include namespace accessors and template information,
        ///     then use is_valid_name_char_ex() instead.
        [[nodiscard]]
        static bool is_valid_name_char(const wchar_t wc) noexcept
            {
            return (i18n_string_util::is_numeric(wc) || i18n_string_util::is_alpha_7bit(wc) ||
                    wc == L'_');
            }

        /// @returns Whether @c wc is an allowable character for function/variable names.
        /// @param wc The character to review.
        /// @note This can include extended parts of a variable/function name to include
        ///     namespace accessor and templates information. For example, this will see
        ///     `std::sort<CString>` as the full name of the function (instead of just sort).
        [[nodiscard]]
        static bool is_valid_name_char_ex(const wchar_t wc) noexcept
            {
            return (i18n_string_util::is_numeric(wc) || i18n_string_util::is_alpha_7bit(wc) ||
                    wc == L'_' || wc == L'.' || wc == L':' || wc == L'<' || wc == L'>');
            }

        /// @brief Strips off decorations from variable and functions.
        /// @details This is language specific and should be reimplemented in derived classes.
        /// @param str The string to strip.
        virtual void remove_decorations([[maybe_unused]] std::wstring& str) const {}

        /// @brief Reviews and classifies a string value based on the
        ///     variable it is being assigned to.
        /// @param variableType The type of the variable being assigned to
        ///     (e.g.,  wxString).
        /// @param variableName The name of the variable being assigned to.
        /// @param value The string value being reviewed.
        /// @param quotePosition The character position of where the
        ///     quote starting @c value begins.
        void process_variable(const std::wstring& variableType, const std::wstring& variableName,
                              const std::wstring& value, const size_t quotePosition);

        /// @brief Fills a block with blanks.
        /// @details Useful for excluding an already processed text block.
        /// @param start The starting position.
        /// @param end The ending position.
        void clear_section(wchar_t* start, const wchar_t* end) const noexcept
            {
            for (ptrdiff_t i = 0; i < end - start; ++i)
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
            @param[out] deprecatedMacroEncountered If a deprecated text macro is encountered,
                will write that to this parameter.
            @param[out] parameterPosition The strings parameter position in the function call.
            @returns The position of the function or variable related to the string.*/
        const wchar_t* read_var_or_function_name(
            const wchar_t* startPos, const wchar_t* const startSentinel, std::wstring& functionName,
            std::wstring& variableName, std::wstring& variableType,
            std::wstring& deprecatedMacroEncountered, size_t& parameterPosition);

        /// @returns @c true if provided variable type is just a decorator after the real
        ///     variable type (e.g., const) and should be skipped.
        /// @param variableType The parsed variable type to review.
        [[nodiscard]]
        virtual bool
        is_variable_type_decorator([[maybe_unused]] const std::wstring_view variableType) const
            {
            return false;
            }

        /// @returns The line and column position from a character position.
        /// @param position The character position in the file.
        /// @param fileStart The start of a file buffer to begin the search from.\n
        ///     If an empty view, will use the currently loaded file.
        [[nodiscard]]
        std::pair<size_t, size_t>
        get_line_and_column(size_t position,
                            std::wstring_view fileStart = std::wstring_view{}) const;

        /// @brief Removes the quotes and whitespace between multiple quotes
        ///     strings that constitute a single string.
        /// @param str The string to collapse.
        /// @returns The collapsed string.
        [[nodiscard]]
        std::wstring collapse_multipart_string(std::wstring str) const;

        /// @brief Collapses non-raw strings that are multiline.
        void process_strings();
        /// @brief Reviews output integrity to see if there were any parsing errors.
        void run_diagnostics();

        const wchar_t* m_file_start{ nullptr };

        bool m_collapse_double_quotes{ false };
        bool m_allow_translating_punctuation_only_strings{ false };
        bool m_exceptions_should_be_translatable{ true };
        bool m_log_messages_are_translatable{ true };
        size_t m_min_words_for_unavailable_string{ 2 };

        int m_min_cpp_version{ 2014 };

        review_style m_reviewStyles{ static_cast<review_style>(
            review_style::check_l10n_strings | review_style::check_suspect_l10n_string_usage |
            review_style::check_not_available_for_l10n | review_style::check_deprecated_macros |
            review_style::check_utf8_encoded | review_style::check_printf_single_number |
            review_style::check_l10n_contains_url | review_style::check_malformed_strings |
            review_style::check_fonts | review_style::all_l10n_checks) };

        // once these are set (by our CTOR and/or by client), they shouldn't be reset
        std::set<std::wstring_view> m_localization_functions;
        std::set<std::wstring_view> m_non_localizable_functions;
        std::set<std::wstring_view> m_internal_functions;
        std::set<std::wstring_view> m_log_functions;
        std::set<std::wstring_view> m_exceptions;
        std::set<std::wstring_view> m_ctors_to_ignore;
        std::set<string_util::case_insensitive_wstring> m_known_internal_strings;
        std::set<std::wstring_view> m_keywords;
        std::map<std::wstring_view, std::wstring> m_deprecated_string_macros;
        std::map<std::wstring_view, std::wstring> m_deprecated_string_functions;
        // These have built-in values, but can be added to by the client also.
        // These are static so that client's additions can propagate to other instances.
        static std::vector<std::wregex> m_variable_name_patterns_to_ignore;
        static std::set<std::wstring> m_variable_types_to_ignore;
        static std::set<string_util::case_insensitive_wstring> m_font_names;
        static std::set<string_util::case_insensitive_wstring> m_file_extensions;
        static std::set<std::wstring> m_untranslatable_exceptions;
        // results after parsing what the client should maybe review
        std::vector<string_info> m_localizable_strings;
        std::vector<string_info> m_marked_as_non_localizable_strings;
        std::vector<string_info> m_internal_strings;
        // results that are probably issues
        std::vector<string_info> m_unsafe_localizable_strings;
        std::vector<string_info> m_localizable_strings_with_urls;
        std::vector<string_info> m_localizable_strings_in_internal_call;
        std::vector<string_info> m_localizable_strings_with_surrounding_spaces;
        std::vector<string_info> m_not_available_for_localization_strings;
        std::vector<string_info> m_deprecated_macros;
        std::vector<string_info> m_unencoded_strings;
        std::vector<string_info> m_printf_single_numbers;
        std::vector<string_info> m_ids_assigned_number;
        std::vector<string_info> m_duplicates_value_assigned_to_ids;
        std::vector<string_info> m_malformed_strings;
        std::vector<string_info> m_trailing_spaces;
        std::vector<string_info> m_tabs;
        std::vector<string_info> m_wide_lines;
        std::vector<string_info> m_comments_missing_space;

        std::filesystem::path m_file_name;

        static const std::wregex m_url_email_regex;
        static const std::wregex m_html_regex;
        static const std::wregex m_html_element_with_content_regex;
        static const std::wregex m_html_tag_regex;
        static const std::wregex m_html_tag_unicode_regex;
        static const std::wregex m_xml_element_regex;
        static const std::wregex m_2letter_regex;
        static const std::wregex m_hashtag_regex;
        static const std::wregex m_key_shortcut_regex;
        static const std::wregex m_function_signature_regex;
        static const std::wregex m_plural_regex;
        static const std::wregex m_open_function_signature_regex;
        static const std::wregex m_diagnostic_function_regex;
        static const std::wregex m_id_assignment_regex;
        static const std::wregex m_sql_code;
        static const std::wregex m_malformed_html_tag;
        static const std::wregex m_malformed_html_tag_bad_amp;
        static const std::wregex m_printf_cpp_int_regex;
        static const std::wregex m_printf_cpp_float_regex;
        static const std::wregex m_printf_cpp_string_regex;
        static const std::wregex m_printf_cpp_pointer_regex;
        static const std::wregex m_file_filter_regex;
        std::vector<std::wregex> m_untranslatable_regexes;
        std::vector<std::wregex> m_translatable_regexes;

      private:
        [[nodiscard]]
        static std::wstring process_po_msg(std::wstring_view msg)
            {
            std::wstring msgId{ msg };

            if (msgId.length() > 0 && msgId.front() == L'"')
                {
                msgId.erase(0, 1);
                }

            string_util::replace_all<std::wstring>(msgId, L"\"\r\n\"", L"");
            string_util::replace_all<std::wstring>(msgId, L"\r\n\"", L"");
            string_util::replace_all<std::wstring>(msgId, L"\"\n\"", L"");
            string_util::replace_all<std::wstring>(msgId, L"\n\"", L"");

            return msgId;
            }

        [[nodiscard]]
        static std::vector<std::wstring>
        convert_positional_cpp_printf(const std::vector<std::wstring>& printfCommands,
                                      std::wstring& errorInfo);

        // helpers
        mutable std::vector<parse_messages> m_error_log;

        // bookkeeping diagnostics
#ifndef NDEBUG
        mutable std::pair<std::wstring, std::wregex> m_longest_internal_string;
#endif
        };
    } // namespace i18n_check

/** @}*/

#endif //__I18N_REVIEW_H__
