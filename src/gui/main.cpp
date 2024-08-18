//////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "datamodel.h"
#include <wx/artprov.h>
#include <wx/dataview.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/wx.h>

//------------------------------------------------------
class I18NFrame : public wxFrame
    {
  public:
    I18NFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {}

    void InitControls();

  private:
    wxObjectDataPtr<I18NResultsTreeModel> m_resultsModel;
    wxDataViewCtrl* m_resultsDataView{ nullptr };
    };

//------------------------------------------------------
void I18NFrame::InitControls()
    {
    wxRibbonBar* m_ribbon =
        new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                        wxRIBBON_BAR_FLOW_HORIZONTAL | wxRIBBON_BAR_SHOW_PAGE_LABELS |
                            wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS | wxRIBBON_BAR_SHOW_TOGGLE_BUTTON |
                            wxRIBBON_BAR_SHOW_HELP_BUTTON);
        {
        wxRibbonPage* homePage = new wxRibbonPage(m_ribbon, wxID_ANY, _(L"Home"));
        wxRibbonPanel* projectPanel =
            new wxRibbonPanel(homePage, wxID_ANY, _(L"Project"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        wxRibbonToolBar* toolbar = new wxRibbonToolBar(projectPanel);
        toolbar->AddTool(wxID_NEW,
                         wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize{ 32, 32 }));
        }

    m_ribbon->Realize();

    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(m_ribbon, wxSizerFlags().Expand());

    m_resultsDataView = new wxDataViewCtrl(this, wxID_ANY);

    m_resultsModel = new I18NResultsTreeModel;
    /*BM m_resultsModel->AddRow("dialog1.rc", "[fontIssue]", "FONT 8,",
                           "Font issue in resource file dialog definition.", 1998, 55);*/
    m_resultsDataView->AssociateModel(m_resultsModel.get());

    // Warning ID
    wxDataViewTextRenderer* tr = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
    m_resultsDataView->AppendColumn(new wxDataViewColumn(
        _(L"Warning ID"), tr, 0, FromDIP(200), wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE));

    // Value
    tr = new wxDataViewTextRenderer(L"string", wxDATAVIEW_CELL_INERT);
    m_resultsDataView->AppendColumn(new wxDataViewColumn(
        _(L"Value"), tr, 1, FromDIP(150), wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE));

    // Line #
    wxDataViewSpinRenderer* sr =
        new wxDataViewSpinRenderer(0, std::numeric_limits<int>::max(), wxDATAVIEW_CELL_INERT,
                                   wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);
    m_resultsDataView->AppendColumn(new wxDataViewColumn(
        _(L"Line #"), sr, 2, FromDIP(100), wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE));

    // Column #
    sr = new wxDataViewSpinRenderer(0, std::numeric_limits<int>::max(), wxDATAVIEW_CELL_INERT,
                                    wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);
    m_resultsDataView->AppendColumn(new wxDataViewColumn(
        _(L"Column #"), sr, 3, FromDIP(100), wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE));

    // Explanation
    tr = new wxDataViewTextRenderer(L"string", wxDATAVIEW_CELL_INERT);
    m_resultsDataView->AppendColumn(new wxDataViewColumn(
        _(L"Explanation"), tr, 4, FromDIP(200), wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE));

    mainSizer->Add(m_resultsDataView, wxSizerFlags(1).Expand());

    SetSizer(mainSizer);
    }

// Application
class I18NApp : public wxApp
    {
  public:
    bool OnInit() final;
    };

wxIMPLEMENT_APP(I18NApp);

//------------------------------------------------------
bool I18NApp::OnInit()
    {
    if (!wxApp::OnInit())
        {
        return false;
        }

    SetAppName(L"i18n-check");

    // create the main application window
    I18NFrame* frame = new I18NFrame(GetAppName());
    frame->InitControls();
    frame->SetSize(frame->FromDIP(wxSize(1200, 900)));
    frame->CenterOnScreen();
    frame->Show(true);

    return true;
    }
