/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_APP_H__
#define __I18N_APP_H__

#include "../analyze.h"
#include "../input.h"
#include "datamodel.h"
#include "projectdlg.h"
#include <wx/artprov.h>
#include <wx/dataview.h>
#include <wx/filename.h>
#include <wx/html/htmlwin.h>
#include <wx/itemattr.h>
#include <wx/numformatter.h>
#include <wx/progdlg.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/splitter.h>
#include <wx/tokenzr.h>
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

//------------------------------------------------------
class I18NFrame : public wxFrame
    {
  public:
    I18NFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {}

    void InitControls();

    void OnNew([[maybe_unused]] wxCommandEvent&);
    void OnEdit([[maybe_unused]] wxCommandEvent&);
    void OnExpandAll([[maybe_unused]] wxCommandEvent&);
    void OnCollapseAll([[maybe_unused]] wxCommandEvent&);

  private:
    void Process();

    void Collapse()
        {
        wxDataViewItemArray array;
        m_resultsModel->GetChildren(m_resultsModel->GetRoot(), array);
        for (const auto& item : array)
            {
            m_resultsDataView->Collapse(item);
            }
        }

    void ExpandAll()
        {
        wxDataViewItemArray array;
        m_resultsModel->GetChildren(m_resultsModel->GetRoot(), array);
        for (const auto& item : array)
            {
            m_resultsDataView->ExpandChildren(item);
            }
        }

    wxObjectDataPtr<I18NResultsTreeModel> m_resultsModel;
    wxDataViewCtrl* m_resultsDataView{ nullptr };
    wxHtmlWindow* m_messageWindow{ nullptr };

    // active project options
    wxString m_filePath;
    wxString m_excludedPaths;
    int m_options{ static_cast<int>(i18n_check::review_style::check_l10n_strings |
                                    i18n_check::review_style::check_suspect_l10n_string_usage |
                                    i18n_check::review_style::check_not_available_for_l10n |
                                    i18n_check::review_style::check_deprecated_macros |
                                    i18n_check::review_style::check_utf8_encoded |
                                    i18n_check::review_style::check_printf_single_number |
                                    i18n_check::review_style::check_l10n_contains_url |
                                    i18n_check::review_style::check_malformed_strings |
                                    i18n_check::review_style::check_fonts |
                                    i18n_check::review_style::all_l10n_checks) };
    bool m_fuzzyTranslations{ false };
    bool m_logMessagesCanBeTranslated{ true };
    bool m_allowTranslatingPunctuationOnlyStrings{ false };
    bool m_exceptionsShouldBeTranslatable{ true };
    int m_minWordsForClassifyingUnavailableString{ 2 };
    int m_minCppVersion{ 14 };
    };

// Application
class I18NApp : public wxApp
    {
  public:
    bool OnInit() final;
    };

    /** @}*/

#endif //__I18N_APP_H__
