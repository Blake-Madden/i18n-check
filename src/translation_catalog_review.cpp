/********************************************************************************
 * Copyright (c) 2024 Blake Madden
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

#include "translation_catalog_review.h"

namespace i18n_check
    {
    //------------------------------------------------
    void translation_catalog_review::review_strings(analyze_callback_reset resetCallback,
                                                    analyze_callback callback)
        {
        std::vector<std::wstring> printfStrings1, printfStrings2;
        std::wstring errorInfo;

        resetCallback(m_catalog_entries.size());
        size_t currentCatalogIndex{ 0 };
        for (auto& catEntry : m_catalog_entries)
            {
            if (!callback(++currentCatalogIndex, std::wstring{}))
                {
                return;
                }
            if (static_cast<bool>(m_review_styles & check_l10n_strings))
                {
                if (is_untranslatable_string(catEntry.second.m_source, false))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source);
                    }
                if (!catEntry.second.m_source_plural.empty() &&
                    is_untranslatable_string(catEntry.second.m_source_plural, false))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source_plural);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_l10n_contains_url))
                {
                std::wsmatch results;

                if (std::regex_search(catEntry.second.m_source, results, m_url_email_regex))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source);
                    }
                if (!catEntry.second.m_source_plural.empty() &&
                    std::regex_search(catEntry.second.m_source_plural, results, m_url_email_regex))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source_plural);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_l10n_has_surrounding_spaces))
                {
                if (has_surrounding_spaces(catEntry.second.m_source))
                    {
                    catEntry.second.m_issues.emplace_back(
                        translation_issue::source_surrounding_spaces_issue,
                        catEntry.second.m_source);
                    }
                if (!catEntry.second.m_source_plural.empty() &&
                    has_surrounding_spaces(catEntry.second.m_source_plural))
                    {
                    catEntry.second.m_issues.emplace_back(
                        translation_issue::source_surrounding_spaces_issue,
                        catEntry.second.m_source_plural);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_needing_context))
                {
                if (catEntry.second.m_comment.empty() &&
                    is_string_ambiguous(catEntry.second.m_source))
                    {
                    catEntry.second.m_issues.emplace_back(
                        translation_issue::source_needing_context_issue,
                                                          catEntry.second.m_source);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_mismatching_printf_commands))
                {
                if (catEntry.second.m_po_format == po_format_string::cpp_format)
                    {
                    // only look at strings that have a translation
                    if (!catEntry.second.m_translation.empty())
                        {
                        printfStrings1 =
                            load_cpp_printf_commands(catEntry.second.m_source, errorInfo);
                        printfStrings2 =
                            load_cpp_printf_commands(catEntry.second.m_translation, errorInfo);

                        if (printfStrings1.size() || printfStrings2.size())
                            {
                            if (printfStrings1 != printfStrings2)
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::printf_issue,
                                    L"'" + catEntry.second.m_source + L"' vs. '" +
                                        catEntry.second.m_translation + L"'" + errorInfo);
                                }
                            }
                        }

                    if (!catEntry.second.m_translation_plural.empty())
                        {
                        printfStrings1 =
                            load_cpp_printf_commands(catEntry.second.m_source_plural, errorInfo);
                        printfStrings2 = load_cpp_printf_commands(
                            catEntry.second.m_translation_plural, errorInfo);

                        if (printfStrings1.size() || printfStrings2.size())
                            {
                            if (printfStrings1 != printfStrings2)
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::printf_issue,
                                    L"'" + catEntry.second.m_source_plural + L"' vs. '" +
                                        catEntry.second.m_translation_plural + L"'" + errorInfo);
                                }
                            }
                        }
                    }
                }

            if (static_cast<bool>(m_review_styles & check_accelerators))
                {
                const std::wregex keyboardAcceleratorRegex{ L"&[[:alnum:]]" };

                if (!catEntry.second.m_translation.empty())
                    {
                    std::wsmatch srcMatches, transMatches;
                    std::regex_search(catEntry.second.m_source, srcMatches,
                                      keyboardAcceleratorRegex);
                    std::regex_search(catEntry.second.m_translation, transMatches,
                                      keyboardAcceleratorRegex);

                    if ((srcMatches.size() == 1 && transMatches.size() != 1) ||
                        (srcMatches.size() != 1 && transMatches.size() == 1))
                        {
                        catEntry.second.m_issues.emplace_back(
                            translation_issue::accelerator_issue,
                            L"'" + catEntry.second.m_source + L"' vs. '" +
                                catEntry.second.m_translation + L"'" + errorInfo);
                        }
                    }

                if (!catEntry.second.m_translation_plural.empty())
                    {
                    std::wsmatch srcMatches, transMatches;
                    std::regex_search(catEntry.second.m_source_plural, srcMatches,
                                      keyboardAcceleratorRegex);
                    std::regex_search(catEntry.second.m_translation_plural, transMatches,
                                      keyboardAcceleratorRegex);

                    if ((srcMatches.size() == 1 && transMatches.size() != 1) ||
                        (srcMatches.size() != 1 && transMatches.size() == 1))
                        {
                        catEntry.second.m_issues.emplace_back(
                            translation_issue::accelerator_issue,
                            L"'" + catEntry.second.m_source_plural + L"' vs. '" +
                                catEntry.second.m_translation_plural + L"'" + errorInfo);
                        }
                    }
                }

            if (static_cast<bool>(m_review_styles & check_consistency))
                {
                if (!catEntry.second.m_source.empty() && !catEntry.second.m_translation.empty())
                    {
                    const wchar_t lastSrcChar{ catEntry.second.m_source.back() };
                    const wchar_t lastTransChar{ catEntry.second.m_translation.back() };

                    const bool srcIsStop{ i18n_string_util::is_period(lastSrcChar) ||
                                          i18n_string_util::is_exclamation(lastSrcChar) ||
                                          i18n_string_util::is_question(lastSrcChar) };
                    const bool transIsStop{ i18n_string_util::is_period(lastTransChar) ||
                                            i18n_string_util::is_exclamation(lastTransChar) ||
                                            i18n_string_util::is_question(lastTransChar) };

                    if ((std::iswspace(lastSrcChar) && !std::iswspace(lastTransChar)) ||
                        (!std::iswspace(lastSrcChar) && std::iswspace(lastTransChar)) ||
                        // not that it is allowable for source to not have full stop, but for
                        // translation too
                        (srcIsStop && !transIsStop))
                        {
                        // if source is an exclamation and the translation is not, then that is OK
                        if (!(i18n_string_util::is_exclamation(lastSrcChar) && !transIsStop) &&
                            // translation ending with ')' is OK also if source has a full stop
                            !(srcIsStop && i18n_string_util::is_close_parenthesis(lastTransChar)))
                            {
                            catEntry.second.m_issues.emplace_back(
                                translation_issue::consistency_issue,
                                L"'" + catEntry.second.m_source + L"' vs. '" +
                                    catEntry.second.m_translation + L"'" + errorInfo);
                            }
                        }
                    else if (std::iswupper(catEntry.second.m_source.front()) &&
                             std::iswlower(catEntry.second.m_translation.front()))
                        {
                        catEntry.second.m_issues.emplace_back(
                            translation_issue::consistency_issue,
                            L"'" + catEntry.second.m_source + L"' vs. '" +
                                catEntry.second.m_translation + L"'" + errorInfo);
                        }
                    }
                }
            }
        }
    } // namespace i18n_check
