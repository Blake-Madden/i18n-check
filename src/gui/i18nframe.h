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

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef __I18N_FRAME_H__
#define __I18N_FRAME_H__

#include "../analyze.h"
#include "../input.h"
#include "app_options.h"
#include "datamodel.h"
#include "projectdlg.h"
#include <wx/aboutdlg.h>
#include <wx/appprogress.h>
#include <wx/artprov.h>
#include <wx/busyinfo.h>
#include <wx/dataview.h>
#include <wx/filename.h>
#include <wx/fontenum.h>
#include <wx/itemattr.h>
#include <wx/notebook.h>
#include <wx/numformatter.h>
#include <wx/progdlg.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/richmsgdlg.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>
#include <wx/stdpaths.h>
#include <wx/textctrl.h>
#include <wx/tokenzr.h>
#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

//------------------------------------------------------
class I18NFrame : public wxFrame
    {
  public:
    explicit I18NFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {}

    void InitControls();

    void OnNew([[maybe_unused]] wxCommandEvent&);
    void OnOpen([[maybe_unused]] wxCommandEvent&);
    void OnSave([[maybe_unused]] wxCommandEvent&);
    void OnExportResults([[maybe_unused]] wxCommandEvent&);
    void OnInsertTranslatorComment([[maybe_unused]] wxCommandEvent&);
    void OnInsertEncodedUnicode([[maybe_unused]] wxCommandEvent&);
    void OnSaveMenu(wxRibbonButtonBarEvent& event);
    void OnRefresh([[maybe_unused]] wxCommandEvent&);
    void OnOpenSelectedFile([[maybe_unused]] wxCommandEvent&);
    void OnIgnore(wxRibbonButtonBarEvent& event);
    void OnInsert(wxRibbonButtonBarEvent& event);
    void OnIgnoreSelectedFile([[maybe_unused]] wxCommandEvent&);
    void OnIgnoreSelectedWarning([[maybe_unused]] wxCommandEvent&);
    void OnSettings([[maybe_unused]] wxCommandEvent&);
    void OnAbout([[maybe_unused]] wxCommandEvent&);
    void OnHelp([[maybe_unused]] wxCommandEvent&);

    void OnClose(wxCloseEvent& event);

  private:
    void OnEditButtonClicked(wxRibbonButtonBarEvent& event);

    void EnableEditBar(const bool enable)
        {
        m_editBar->EnableButton(wxID_UNDO, enable);
        m_editBar->EnableButton(wxID_REDO, enable);
        m_editBar->EnableButton(wxID_PASTE, enable);
        m_editBar->EnableButton(wxID_CUT, enable);
        m_editBar->EnableButton(wxID_COPY, enable);
        m_editBar->EnableButton(XRCID("ID_INSERT"), enable);
        m_editBar->EnableButton(wxID_SELECTALL, enable);
        }

    void Process();

    /// @brief Copies select options (i.e., ignored variable patterns) from a project to
    ///     the global options.
    void CopyProjectOptionsToGlobalOptions();

    void ExpandAll()
        {
        wxDataViewItemArray array;
        m_resultsModel->GetChildren(m_resultsModel->GetRoot(), array);
        for (const auto& item : array)
            {
            m_resultsDataView->ExpandChildren(item);
            }
        }

    void SetTitleDirty()
        {
        wxString title{ GetTitle() };
        if (!title.ends_with(L"*"))
            {
            title.append(L"*");
            }
        SetTitle(std::move(title));
        }

    void SaveSourceFileIfNeeded();
    void SaveProjectIfNeeded();

    wxObjectDataPtr<I18NResultsTreeModel> m_resultsModel;
    wxDataViewCtrl* m_resultsDataView{ nullptr };
    wxRibbonButtonBar* m_projectBar{ nullptr };
    wxRibbonButtonBar* m_editBar{ nullptr };
    wxStyledTextCtrl* m_editor{ nullptr };
    wxTextCtrl* m_logWindow{ nullptr };
    wxString m_activeSourceFile;
    bool m_promptForFileSave{ true };

    bool m_projectDirty{ false };
    wxString m_activeProjectFilePath;

    wxString m_activeResults;

    constexpr static wxWindowID DATA_VIEW = wxID_HIGHEST;
    constexpr static wxWindowID EDITOR_ID = DATA_VIEW + 1;

    constexpr static int ERROR_ANNOTATION_STYLE = wxSTC_STYLE_LASTPREDEFINED + 1;

    I18NOptions m_activeProjectOptions;
    bool m_hasOpenProject{ false };
    };

    /** @}*/

#endif //__I18N_FRAME_H__
