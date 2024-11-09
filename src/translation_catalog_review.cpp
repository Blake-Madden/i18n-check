///////////////////////////////////////////////////////////////////////////////
// Name:        translation_catalog_review.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

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
            if (static_cast<bool>(m_reviewStyles & check_l10n_strings))
                {
                if (is_untranslatable_string(catEntry.second.m_source, false))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source);
                    }
                }
            if (m_reviewStyles & check_l10n_contains_url)
                {
                std::wsmatch results;

                if (std::regex_search(catEntry.second.m_source, results, m_url_email_regex))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source);
                    }
                }
            if (static_cast<bool>(m_reviewStyles & check_mismatching_printf_commands))
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
                                    L"\"" + catEntry.second.m_source + L"\" vs. \"" +
                                        catEntry.second.m_translation + L"\"" + errorInfo);
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
                                    L"\"" + catEntry.second.m_source_plural + L"\" vs. \"" +
                                        catEntry.second.m_translation_plural + L"\"" + errorInfo);
                                }
                            }
                        }
                    }
                }

            if (static_cast<bool>(m_reviewStyles & check_accelerators))
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
                            L"\"" + catEntry.second.m_source + L"\" vs. \"" +
                                catEntry.second.m_translation + L"\"" + errorInfo);
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
                            L"\"" + catEntry.second.m_source_plural + L"\" vs. \"" +
                                catEntry.second.m_translation_plural + L"\"" + errorInfo);
                        }
                    }
                }
            }
        }
    } // namespace i18n_check
