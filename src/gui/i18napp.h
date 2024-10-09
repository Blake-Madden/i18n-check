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
#include "app_options.h"
#include "datamodel.h"
#include "projectdlg.h"
#include <wx/aboutdlg.h>
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
class I18NArtProvider final : public wxArtProvider
    {
  public:
    I18NArtProvider();

  protected:
    [[nodiscard]]
    wxBitmapBundle CreateBitmapBundle(const wxArtID& id, const wxArtClient& client,
                                      const wxSize& size) final;

    [[nodiscard]]
    wxBitmapBundle GetSVG(const wxString& path) const;

  private:
    std::map<wxArtID, wxString> m_idFileMap;
    };

//------------------------------------------------------
class I18NFrame : public wxFrame
    {
  public:
    I18NFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {}

    void InitControls();

    void OnNew([[maybe_unused]] wxCommandEvent&);
    void OnOpen([[maybe_unused]] wxCommandEvent&);
    void OnSave([[maybe_unused]] wxCommandEvent&);
    void OnRefresh([[maybe_unused]] wxCommandEvent&);
    void OnExpandAll([[maybe_unused]] wxCommandEvent&);
    void OnCollapseAll([[maybe_unused]] wxCommandEvent&);
    void OnIgnoreSelectedFile([[maybe_unused]] wxCommandEvent&);
    void OnSettings([[maybe_unused]] wxCommandEvent&);
    void OnAbout([[maybe_unused]] wxCommandEvent&);

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

    void SetTitleDirty()
        {
        wxString title{ GetTitle() };
        if (title.ends_with(L"*"))
            {
            title.erase(title.length() - 1);
            }
        title.append(L"*");
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

    constexpr static int ERROR_ANNOTATION_STYLE = wxSTC_STYLE_LASTPREDEFINED + 1;

    I18NOptions m_activeProjectOptions;
    bool m_hasOpenProject{ false };
    };

// Application
class I18NApp : public wxApp
    {
  public:
    bool OnInit() final;
    int OnExit() final;

    I18NOptions m_defaultOptions;
    wxString m_optionsFilePath;
    };

    /** @}*/

#endif //__I18N_APP_H__
