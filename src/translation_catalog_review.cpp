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
    void translation_catalog_review::review_strings()
        {
        std::vector<std::wstring> printfStrings1, printfStrings2;
        std::wstring errorInfo;
        for (auto& catEntry : m_catalog_entries)
            {
            if (static_cast<bool>(m_reviewStyles & check_l10n_strings))
                {
                std::wstring originalStr{ catEntry.second.m_source };
                if (is_untranslatable_string(originalStr, false))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source);
                    }
                }
            if (m_reviewStyles & check_l10n_contains_url)
                {
                std::wsmatch results;

                if (std::regex_search(catEntry.second.m_source, results, m_urlEmailRE))
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
            }
        }
    } // namespace i18n_check
