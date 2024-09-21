/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_APPOPTIONS_H__
#define __I18N_APPOPTIONS_H__

#include "../i18n_review.h"
#include "../pseudo_translate.h"
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

//------------------------------------------------------
struct I18NOptions
    {
    void Save(const wxString& filePath);
    void Load(const wxString& filePath);

    wxString m_filePath;
    wxString m_excludedPaths;
    int64_t m_options{ static_cast<int64_t>(
        i18n_check::review_style::check_l10n_strings |
        i18n_check::review_style::check_suspect_l10n_string_usage |
        i18n_check::review_style::check_not_available_for_l10n |
        i18n_check::review_style::check_deprecated_macros |
        i18n_check::review_style::check_utf8_encoded |
        i18n_check::review_style::check_printf_single_number |
        i18n_check::review_style::check_l10n_contains_url |
        i18n_check::review_style::check_malformed_strings | i18n_check::review_style::check_fonts |
        i18n_check::review_style::all_l10n_checks) };
    bool m_fuzzyTranslations{ false };
    i18n_check::pseudo_translation_method m_pseudoTranslationMethod{
        i18n_check::pseudo_translation_method::none
    };
    bool m_addPseudoTransBrackets{ false };
    bool m_pseudoTrack{ false };
    bool m_logMessagesCanBeTranslated{ true };
    bool m_allowTranslatingPunctuationOnlyStrings{ false };
    bool m_exceptionsShouldBeTranslatable{ true };
    int m_widthPseudoIncrease{ 0 };
    int m_minWordsForClassifyingUnavailableString{ 2 };
    int m_minCppVersion{ 14 };
    };

    /** @}*/

#endif //__I18N_APPOPTIONS_H__
