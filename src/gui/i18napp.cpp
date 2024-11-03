//////////////////////////////////////////////////////////////////////////////
// Name:        i18napp.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "i18napp.h"
#include <iostream>
#include <wx/uilocale.h>

wxIMPLEMENT_APP(I18NApp);

I18NArtProvider::I18NArtProvider()
    {
    // cppcheck-suppress useInitializationList
    m_idFileMap = { { wxART_FILE_OPEN, L"images/file-open.svg" },
                    { wxART_FILE_SAVE, L"images/file-save.svg" },
                    { wxART_NEW, L"images/document.svg" },
                    { wxART_REFRESH, L"images/reload.svg" },
                    { wxART_DELETE, L"images/delete.svg" },
                    { wxART_HELP, L"images/help.svg" },
                    { L"ID_CODE", L"images/code.svg" },
                    { L"ID_TRANSLATIONS", L"images/translations.svg" },
                    { L"ID_CHECK", L"images/check.svg" },
                    { L"ID_FORMAT", L"images/format.svg" },
                    { L"ID_DEBUG", L"images/bug.svg" },
                    { L"ID_SETTINGS", L"images/project-settings.svg" },
                    { L"ID_ABOUT", L"images/app-logo.svg" } };
    }

//-------------------------------------------
wxBitmapBundle I18NArtProvider::CreateBitmapBundle(const wxArtID& id, const wxArtClient& client,
                                                   const wxSize& size)
    {
    const auto filePath = m_idFileMap.find(id);

    return (filePath != m_idFileMap.cend()) ? GetSVG(filePath->second) :
                                              wxArtProvider::CreateBitmapBundle(id, client, size);
    }

//-------------------------------------------------------
wxBitmapBundle I18NArtProvider::GetSVG(const wxString& path) const
    {
    // load bitmap from disk if a local file
    if (wxFile::Exists(path))
        {
        wxASSERT_MSG(wxBitmapBundle::FromSVGFile(path, wxSize(16, 16)).IsOk(),
                     L"Failed to load SVG icon!");

        wxVector<wxBitmap> bmps;
        bmps.push_back(wxBitmapBundle::FromSVGFile(path, wxSize(16, 16)).GetBitmap(wxSize(16, 16)));
        bmps.push_back(wxBitmapBundle::FromSVGFile(path, wxSize(32, 32)).GetBitmap(wxSize(32, 32)));
        bmps.push_back(wxBitmapBundle::FromSVGFile(path, wxSize(64, 64)).GetBitmap(wxSize(64, 64)));
        bmps.push_back(
            wxBitmapBundle::FromSVGFile(path, wxSize(128, 128)).GetBitmap(wxSize(128, 128)));

        return wxBitmapBundle::FromBitmaps(bmps);
        }

    return wxBitmapBundle{};
    }

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
        m_projectBar = new wxRibbonButtonBar(projectPanel);
        m_projectBar->AddButton(wxID_NEW, _(L"New"),
                                wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize{ 32, 32 }));
        m_projectBar->AddButton(
            wxID_OPEN, _(L"Open"),
            wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize{ 32, 32 }));
        m_projectBar->AddButton(
            wxID_SAVE, _(L"Save"),
            wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, wxSize{ 32, 32 }));
        m_projectBar->AddButton(
            wxID_REFRESH, _(L"Refresh"),
            wxArtProvider::GetBitmap(wxART_REFRESH, wxART_OTHER, wxSize{ 32, 32 }));
        m_projectBar->AddButton(
            XRCID("ID_IGNORE_SELECTED"), _(L"Ignore Selected File"),
            wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER, wxSize{ 32, 32 }));
        m_projectBar->EnableButton(wxID_SAVE, false);
        m_projectBar->EnableButton(wxID_REFRESH, false);
        m_projectBar->EnableButton(XRCID("ID_IGNORE_SELECTED"), false);

        wxRibbonPanel* viewPanel =
            new wxRibbonPanel(homePage, wxID_ANY, _(L"View"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        m_editBar = new wxRibbonButtonBar(viewPanel);
        m_editBar->AddButton(XRCID("ID_EXPAND_ALL"), _(L"Expand All"),
                             wxArtProvider::GetBitmap(wxART_MINUS, wxART_OTHER, wxSize{ 32, 32 }));
        m_editBar->AddButton(XRCID("ID_COLLAPSE_ALL"), _(L"Collapse All"),
                             wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize{ 32, 32 }));
        m_editBar->EnableButton(XRCID("ID_EXPAND_ALL"), false);
        m_editBar->EnableButton(XRCID("ID_COLLAPSE_ALL"), false);

        wxRibbonPanel* helpPanel =
            new wxRibbonPanel(homePage, wxID_ANY, _(L"General"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* toolbar = new wxRibbonButtonBar(helpPanel);
        toolbar->AddButton(XRCID("ID_SETTINGS"), _(L"Settings"),
                           wxArtProvider::GetBitmap(L"ID_SETTINGS", wxART_OTHER, wxSize{ 32, 32 }));
        toolbar->AddButton(wxID_HELP, _(L"Help"),
                           wxArtProvider::GetBitmap(wxART_HELP, wxART_OTHER, wxSize{ 32, 32 }));
        toolbar->AddButton(wxID_ABOUT, _(L"About"),
                           wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, wxSize{ 32, 32 }));
        }

    if (wxSystemSettings::GetAppearance().IsDark())
        {
        m_ribbon->GetArtProvider()->SetColourScheme(
            wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE),
            wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW), wxColour(253, 211, 155));

        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_BUTTON_BAR_LABEL_DISABLED_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_TOP_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_TOP_COLOUR,
            wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_COLOUR,
                                              *wxBLACK);
        m_ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR,
            wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_TAB_ACTIVE_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_PANEL_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR, *wxBLACK);
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_TAB_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_TAB_HOVER_LABEL_COLOUR, *wxBLACK);
        }

    m_ribbon->Realize();

    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(m_ribbon, wxSizerFlags{}.Expand());

    wxSplitterWindow* splitter = new wxSplitterWindow(this);

    m_resultsDataView = new wxDataViewCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                           wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_SINGLE);

    m_resultsModel = new I18NResultsTreeModel;

    m_resultsDataView->AssociateModel(m_resultsModel.get());

    // Warning ID
    wxDataViewIconTextRenderer* tir = new wxDataViewIconTextRenderer();
    m_resultsDataView->AppendColumn(
        new wxDataViewColumn(_(L"Warning ID"), tir, 0, FromDIP(200), wxALIGN_LEFT,
                             wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE));

    // Value
    wxDataViewTextRenderer* tr = new wxDataViewTextRenderer();
    m_resultsDataView->AppendColumn(
        new wxDataViewColumn(_(L"Value"), tr, 1, FromDIP(150), wxALIGN_LEFT,
                             wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE));

    // Line #
    wxDataViewSpinRenderer* sr =
        new wxDataViewSpinRenderer(0, std::numeric_limits<int>::max(), wxDATAVIEW_CELL_INERT,
                                   wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);
    m_resultsDataView->AppendColumn(
        new wxDataViewColumn(_(L"Line"), sr, 2, FromDIP(100), wxALIGN_LEFT,
                             wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE));

    // Column #
    sr = new wxDataViewSpinRenderer(0, std::numeric_limits<int>::max(), wxDATAVIEW_CELL_INERT,
                                    wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL);
    m_resultsDataView->AppendColumn(
        new wxDataViewColumn(_(L"Column"), sr, 3, FromDIP(100), wxALIGN_LEFT,
                             wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE));

    // Explanation
    tr = new wxDataViewTextRenderer();
    m_resultsDataView->AppendColumn(
        new wxDataViewColumn(_(L"Summary"), tr, 4, FromDIP(200), wxALIGN_LEFT,
                             wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE));

    wxNotebook* tabstrip =
        new wxNotebook(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);

    m_editor = new wxStyledTextCtrl(tabstrip);
    m_editor->StyleClearAll();
    const wxFont font{ wxFontInfo().Family(wxFONTFAMILY_MODERN) };
    for (auto i = 0; i < wxSTC_STYLE_LASTPREDEFINED; ++i)
        {
        m_editor->StyleSetFont(i, font.Larger());
        }

    // code-folding options
    m_editor->SetProperty(L"fold", L"1");
    m_editor->SetProperty(L"fold.comment", L"1");
    m_editor->SetProperty(L"fold.compact", L"1");
    const wxColour markerColor{ wxSystemSettings::SelectLightDark(*wxWHITE, *wxBLACK) };
    const wxColour markerContrastColor{ wxSystemSettings::SelectLightDark(*wxBLACK, *wxWHITE) };
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_DOTDOTDOT, markerColor, markerColor);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, markerColor,
                           markerColor);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY, markerColor, markerColor);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_DOTDOTDOT, markerColor,
                           markerContrastColor);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, markerColor,
                           markerContrastColor);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY, markerColor, markerColor);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY, markerColor, markerColor);
    // margin settings
    m_editor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_editor->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
    m_editor->SetMarginMask(1, wxSTC_MASK_FOLDERS);
    m_editor->SetMarginWidth(0, FromDIP(50));
    m_editor->SetMarginWidth(1, FromDIP(16));
    m_editor->SetMarginSensitive(1, true);
    m_editor->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
                           wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
    // turn off tabs
    m_editor->SetUseTabs(false);
    m_editor->SetTabWidth(4);
    // enable auto-completion
    m_editor->AutoCompSetIgnoreCase(true);
    m_editor->AutoCompSetAutoHide(true);
    // annotations styles
#if wxCHECK_VERSION(3, 3, 0)
    m_editor->StyleSetBackground(
        ERROR_ANNOTATION_STYLE,
        wxSystemSettings::SelectLightDark(wxColour(244, 220, 220), wxColour(100, 100, 100)));
#endif
    m_editor->StyleSetSizeFractional(
        ERROR_ANNOTATION_STYLE, (m_editor->StyleGetSizeFractional(wxSTC_STYLE_DEFAULT) * 4) / 5);
    // turn on annotations
    m_editor->AnnotationSetVisible(wxSTC_ANNOTATION_BOXED);

    const wxColour grey(100, 100, 100);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDER, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDER, grey);

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, grey);

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDERSUB, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDERSUB, grey);

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_ARROW);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, markerContrastColor);

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, markerContrastColor);

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDERMIDTAIL, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDERMIDTAIL, grey);

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDERTAIL, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDERTAIL, grey);

    m_editor->SetWrapMode(wxSTC_WRAP_NONE);

    m_editor->StyleSetForeground(wxSTC_C_STRING, wxSystemSettings::SelectLightDark(
                                                     wxColour(150, 0, 0), wxColour(L"#FFA089")));
    m_editor->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(165, 105, 0));
    m_editor->StyleSetForeground(
        wxSTC_C_IDENTIFIER,
        wxSystemSettings::SelectLightDark(wxColour(40, 0, 60), wxColour(L"#CB99C9")));
    m_editor->StyleSetForeground(wxSTC_C_NUMBER, wxSystemSettings::SelectLightDark(
                                                     wxColour(0, 150, 0), wxColour(L"#00CC99")));
    m_editor->StyleSetForeground(wxSTC_C_CHARACTER, wxSystemSettings::SelectLightDark(
                                                        wxColour(150, 0, 0), wxColour(L"#FFA089")));
    m_editor->StyleSetForeground(
        wxSTC_C_WORD, wxSystemSettings::SelectLightDark(wxColour(0, 0, 150), wxColour(L"#0095B6")));
    m_editor->StyleSetForeground(wxSTC_C_WORD2, wxSystemSettings::SelectLightDark(
                                                    wxColour(0, 150, 0), wxColour(L"#00CC99")));

    const wxColour commentColor{ wxSystemSettings::SelectLightDark(wxColour{ L"#499141" },
                                                                   wxColour{ L"#53A64A" }) };

    m_editor->StyleSetForeground(wxSTC_C_COMMENT, commentColor);
    m_editor->StyleSetForeground(wxSTC_C_COMMENTLINE, commentColor);
    m_editor->StyleSetForeground(wxSTC_C_COMMENTDOC, commentColor);
    m_editor->StyleSetBold(wxSTC_C_WORD, true);
    m_editor->StyleSetBold(wxSTC_C_WORD2, true);
    m_editor->StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD, true);

    m_editor->CallTipUseStyle(40);

    m_logWindow = new wxTextCtrl(tabstrip, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                                 wxTE_RICH2 | wxTE_READONLY | wxTE_MULTILINE | wxBORDER_THEME);
    if (wxSystemSettings::GetAppearance().IsDark())
        {
        m_logWindow->SetBackgroundColour(
            wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_BACKGROUND));
        m_logWindow->SetForegroundColour(
            wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_BTNTEXT));
        }

    tabstrip->InsertPage(0, m_editor, _("Edit"));
    tabstrip->InsertPage(1, m_logWindow, _("Analysis Log"), false);

    splitter->SplitHorizontally(m_resultsDataView, tabstrip, FromDIP(-300));
    mainSizer->Add(splitter, wxSizerFlags{ 1 }.Expand());

    SetSizer(mainSizer);

    wxAcceleratorEntry accelEntries[5];
    accelEntries[0].Set(wxACCEL_NORMAL, WXK_F1, wxID_HELP);
    accelEntries[1].Set(wxACCEL_CMD, static_cast<int>(L'N'), wxID_NEW);
    accelEntries[2].Set(wxACCEL_CMD, static_cast<int>(L'O'), wxID_OPEN);
    accelEntries[3].Set(wxACCEL_CMD, static_cast<int>(L'S'), wxID_SAVE);
    accelEntries[4].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);
    wxAcceleratorTable accelTable(std::size(accelEntries), accelEntries);
    SetAcceleratorTable(accelTable);

    Bind(wxEVT_CLOSE_WINDOW,
         [this](wxCloseEvent& event)
         {
             SaveSourceFileIfNeeded();
             SaveProjectIfNeeded();
             event.Skip();
         });
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnNew, this, wxID_NEW);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnRefresh, this, wxID_REFRESH);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnExpandAll, this, XRCID("ID_EXPAND_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnCollapseAll, this, XRCID("ID_COLLAPSE_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnIgnoreSelectedFile, this,
         XRCID("ID_IGNORE_SELECTED"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnSettings, this, XRCID("ID_SETTINGS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnHelp, this, wxID_HELP);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnSettings(event);
        },
        XRCID("ID_SETTINGS"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnHelp(event);
        },
        wxID_HELP);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnAbout(event);
        },
        wxID_ABOUT);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnIgnoreSelectedFile(event);
        },
        XRCID("ID_IGNORE_SELECTED"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnExpandAll(event);
        },
        XRCID("ID_COLLAPSE_ALL"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnExpandAll(event);
        },
        XRCID("ID_EXPAND_ALL"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnNew(event);
        },
        wxID_NEW);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnOpen(event);
        },
        wxID_OPEN);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnSave(event);
        },
        wxID_SAVE);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnRefresh(event);
        },
        wxID_REFRESH);
    Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED,
         [this](wxDataViewEvent& event)
         {
             I18NResultsTreeModelNode* node =
                 reinterpret_cast<I18NResultsTreeModelNode*>(event.GetItem().GetID());
             if (node != nullptr)
                 {
                 wxLaunchDefaultApplication(node->m_fileName);
                 }
         });
    Bind(wxEVT_DATAVIEW_SELECTION_CHANGED,
         [this](wxDataViewEvent& event)
         {
             I18NResultsTreeModelNode* node =
                 reinterpret_cast<I18NResultsTreeModelNode*>(event.GetItem().GetID());

             if (node != nullptr)
                 {
                 // selecting file name root node
                 if (node->m_fileName == node->m_warningId)
                     {
                     SaveSourceFileIfNeeded();
                     m_activeSourceFile.clear();
                     m_editor->SetText(wxString{});
                     return;
                     }

                 // only prompt about saving if file is different
                 if (m_activeSourceFile != node->m_fileName)
                     {
                     SaveSourceFileIfNeeded();

                     const wxString fileExt = wxFileName{ node->m_fileName }.GetExt();

                     if (fileExt.CmpNoCase(L"cpp") == 0 || fileExt.CmpNoCase(L"c") == 0 ||
                         fileExt.CmpNoCase(L"h") == 0 || fileExt.CmpNoCase(L"hpp") == 0)
                         {
                         m_editor->SetLexer(wxSTC_LEX_CPP);
                         m_editor->SetKeyWords(
                             0,
                             (L"alignas alignof and_eq asm atomic_cancel atomic_commit "
                              L"atomic_noexcept auto "
                              "bitand bitor bool break case catch char char8_t char16_t char32_t "
                              "class compl "
                              "concept const consteval constexpr constinit const_cast continue "
                              "co_await "
                              "co_return co_yield decltype default delete do double dynamic_cast "
                              "else enum "
                              "explicit export extern false float for friend goto if inline int "
                              "long mutable "
                              "namespace new noexcept not not_eq nullptr operator or or_eq private "
                              "protected "
                              "public reflexpr register reinterpret_cast requires return short "
                              "signed "
                              "sizeof static static_assert static_cast struct switch synchronized "
                              "template "
                              "this thread_local throw true try typedef typeid typename "
                              "union unsigned using virtual void volatile wchar_t while xor xor_eq "
                              "final override import module"));
                         }
                     else if (fileExt.CmpNoCase(L"po") == 0)
                         {
                         m_editor->SetLexer(wxSTC_LEX_PO);
                         m_editor->SetKeyWords(0, _DT(L"msgid msgstr msgid_plural"));
                         }
                     else
                         {
                         m_editor->SetLexer(wxSTC_LEX_NULL);
                         m_editor->SetKeyWords(0, wxString{});
                         }

                     m_activeSourceFile = node->m_fileName;
                     m_editor->LoadFile(node->m_fileName);
                     }

                 if (node->m_line != -1)
                     {
                     // if scrolling up, then step back up an extra line so that we can see it
                     // after adding the annotation beneath it
                     m_editor->GotoLine((m_editor->GetFirstVisibleLine() < node->m_line) ?
                                            node->m_line :
                                            node->m_line - 1);
                     m_editor->AnnotationSetText(node->m_line - 1, node->m_explaination);
                     m_editor->AnnotationSetStyle(node->m_line - 1, ERROR_ANNOTATION_STYLE);

                     // Scintilla doesn't update the scroll width for annotations, even with
                     // scroll width tracking on, so do it manually.
                     const int width = m_editor->GetScrollWidth();

                     // Take into account the fact that the annotation is shown indented, with
                     // the same indent as the line it's attached to.
                     // Also, add 3; this is just a hack to account for the width of the box, there
                     // doesn't seem to be any way to get it directly from Scintilla.
                     const int indent = m_editor->GetLineIndentation(node->m_line - 1) + FromDIP(3);

                     const int widthAnn = m_editor->TextWidth(
                         ERROR_ANNOTATION_STYLE, node->m_explaination + wxString(indent, L' '));

                     if (widthAnn > width)
                         {
                         m_editor->SetScrollWidth(widthAnn);
                         }
                     m_editor->SetFocus();
                     }
                 }
             else
                 {
                 SaveSourceFileIfNeeded();
                 m_activeSourceFile.clear();
                 m_editor->SetText(wxString{});
                 }
         });
    }

//------------------------------------------------------
void I18NFrame::OnExpandAll([[maybe_unused]] wxCommandEvent&) { ExpandAll(); }

//------------------------------------------------------
void I18NFrame::OnCollapseAll([[maybe_unused]] wxCommandEvent&) { Collapse(); }

//------------------------------------------------------
void I18NFrame::OnIgnoreSelectedFile([[maybe_unused]] wxCommandEvent&)
    {
    wxDataViewItem selectedItem = m_resultsDataView->GetSelection();

    if (selectedItem.IsOk())
        {
        I18NResultsTreeModelNode* node =
            reinterpret_cast<I18NResultsTreeModelNode*>(selectedItem.GetID());

        if (node != nullptr)
            {
            SaveSourceFileIfNeeded();
            m_activeSourceFile.clear();
            m_editor->SetText(wxString{});

            // child node of file parent node
            if (node->m_fileName != node->m_warningId)
                {
                selectedItem = m_resultsModel->GetParent(selectedItem);
                if (!selectedItem.IsOk())
                    {
                    return;
                    }
                node = reinterpret_cast<I18NResultsTreeModelNode*>(selectedItem.GetID());
                if (node == nullptr)
                    {
                    return;
                    }
                }

            m_activeProjectOptions.m_excludedPaths.push_back(node->m_fileName.wc_str());

            m_resultsModel->Delete(selectedItem);
            m_resultsModel->Cleared();

            m_projectDirty = true;

            SetTitleDirty();
            }
        }
    }

//------------------------------------------------------
void I18NFrame::OnSettings([[maybe_unused]] wxCommandEvent&)
    {
    NewProjectDialog projDlg(this, wxID_ANY, _(L"Default Settings"), false);
    projDlg.SetAllOptions(wxGetApp().m_defaultOptions);

    if (projDlg.ShowModal() == wxID_OK)
        {
        wxGetApp().m_defaultOptions = projDlg.GetAllOptions();
        }
    }

//------------------------------------------------------
void I18NFrame::OnHelp([[maybe_unused]] wxCommandEvent&)
    {
    wxLaunchDefaultApplication(wxStandardPaths::Get().GetResourcesDir() +
                               wxFileName::GetPathSeparator() + L"cuneiform.pdf");
    }

//------------------------------------------------------
void I18NFrame::OnAbout([[maybe_unused]] wxCommandEvent&)
    {
    wxDateTime buildDate;
    buildDate.ParseDate(__DATE__);

    wxAboutDialogInfo aboutInfo;
    aboutInfo.AddDeveloper(L"Blake Madden");
    aboutInfo.SetLicence(_DT(LR"(BSD 3-Clause License

Copyright (c) 2021-2024, Blake-Madden
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)"));
    aboutInfo.SetCopyright(
        wxString::Format(_(L"Copyright \U000000A92021-%d %s. All rights reserved."),
                         buildDate.GetYear(), L"Blake Madden"));
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, wxSize{ 32, 32 }));
    aboutInfo.SetIcon(appIcon);
    wxAboutBox(aboutInfo, this);
    }

//------------------------------------------------------
void I18NFrame::OnRefresh([[maybe_unused]] wxCommandEvent&)
    {
    if (!m_hasOpenProject)
        {
        return;
        }

    NewProjectDialog projDlg(this, wxID_ANY,
                             wxString::Format(_("Edit Project - %s"),
                                              wxFileName{ m_activeProjectFilePath }.GetFullName()));
    projDlg.SetAllOptions(m_activeProjectOptions);

    if (projDlg.ShowModal() == wxID_OK)
        {
        SaveSourceFileIfNeeded();

        m_activeProjectOptions = projDlg.GetAllOptions();

        Process();

        m_projectDirty = true;

        SetTitleDirty();
        }
    }

//------------------------------------------------------
void I18NFrame::OnNew([[maybe_unused]] wxCommandEvent&)
    {
    SaveProjectIfNeeded();

    NewProjectDialog projDlg(this);
    projDlg.SetAllOptions(wxGetApp().m_defaultOptions);

    if (projDlg.ShowModal() == wxID_CANCEL)
        {
        return;
        }

    m_activeProjectFilePath.clear();
    m_projectDirty = true;

    m_activeProjectOptions = projDlg.GetAllOptions();

    SetTitle(wxGetApp().GetAppName() + _(L" - Untitled"));

    Process();
    }

//------------------------------------------------------
void I18NFrame::OnOpen([[maybe_unused]] wxCommandEvent&)
    {
    SaveProjectIfNeeded();

    wxFileDialog dialog(
        this, _(L"Select Project to Open"), wxString{}, wxString{},
        wxString::Format(_(L"%s Project Files (*.xml)|*.xml"), wxGetApp().GetAppName()),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_activeProjectFilePath = dialog.GetPath();
    m_projectDirty = false;

    m_activeProjectOptions.Load(m_activeProjectFilePath);

    SetTitle(wxGetApp().GetAppName() + L" - " +
             wxFileName{ m_activeProjectFilePath }.GetFullName());

    Process();
    }

//------------------------------------------------------
void I18NFrame::OnSave([[maybe_unused]] wxCommandEvent&)
    {
    SaveSourceFileIfNeeded();

    if (!m_hasOpenProject)
        {
        return;
        }

    if (m_activeProjectFilePath.empty())
        {
        const wxFileName projectName{ m_activeProjectOptions.m_filePath };
        const wxString lastFolder =
            projectName.GetName().empty() ? wxString{ _(L"Project") } : projectName.GetName();
        wxFileDialog dialog(
            nullptr, _(L"Save Project"), wxString{}, lastFolder + L".xml",
            wxString::Format(_(L"%s Project Files (*.xml)|*.xml"), wxGetApp().GetAppName()),
            wxFD_SAVE | wxFD_PREVIEW | wxFD_OVERWRITE_PROMPT);
        if (dialog.ShowModal() != wxID_OK)
            {
            return;
            }

        m_activeProjectFilePath = dialog.GetPath();
        }

    m_activeProjectOptions.Save(m_activeProjectFilePath);

    SetTitle(wxGetApp().GetAppName() + L" - " +
             wxFileName{ m_activeProjectFilePath }.GetFullName());

    m_projectDirty = false;
    }

//------------------------------------------------------
void I18NFrame::SaveProjectIfNeeded()
    {
    if (m_projectDirty)
        {
        if (wxMessageBox(_(L"Do you wish to save changes to the project?"), _(L"Save Project"),
                         wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
            {
            wxRibbonButtonBarEvent event;
            OnSave(event);
            m_projectDirty = false;
            }
        }
    }

//------------------------------------------------------
void I18NFrame::SaveSourceFileIfNeeded()
    {
    const auto saveFile = [this]()
    {
        // Windows RC files are usually ANSI, so get the code page in them
        // and save with that encoding
        if (wxFileName{ m_activeSourceFile }.GetExt().CmpNoCase(L"rc") == 0)
            {
            std::wstring encoding{ L"utf-8" };
            const std::wstring fileText = m_editor->GetText().wc_str();
            const std::wregex codePageRE{ _DT(LR"(#pragma code_page\(([0-9]+)\))") };
            std::wsmatch matchResults;
            if (std::regex_search(fileText.cbegin(), fileText.cend(), matchResults, codePageRE) &&
                matchResults.size() >= 2)
                {
                encoding = _DT(L"Windows-") + matchResults.str(1);
                }
            wxFile outFile{ m_activeSourceFile, wxFile::write };
            if (outFile.IsOpened())
                {
                outFile.Write(fileText, wxCSConv(encoding));
                }
            }
        else
            {
            m_editor->SaveFile(m_activeSourceFile);
            }
    };
    if (!m_activeSourceFile.empty() && m_editor->IsModified())
        {
        if (m_promptForFileSave)
            {
            wxRichMessageDialog msg(
                this,
                wxString::Format(_(L"Do you wish to save changes to '%s'?"),
                                 wxFileName{ m_activeSourceFile }.GetFullName()),
                _(L"Remove Test"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
            msg.SetEscapeId(wxID_NO);
            msg.ShowCheckBox(_(L"Always save without prompting"));
            const int dlgResponse = msg.ShowModal();
            // save the checkbox status
            if (msg.IsCheckBoxChecked() && (dlgResponse == wxID_YES))
                {
                m_promptForFileSave = false;
                }
            // now see if they said "Yes" or "No"
            if (dlgResponse == wxID_YES)
                {
                saveFile();
                }
            }
        else
            {
            saveFile();
            }
        }
    }

//------------------------------------------------------
void I18NFrame::Process()
    {
    SaveSourceFileIfNeeded();

    m_activeSourceFile.clear();
    m_editor->SetText(wxString{});

    std::filesystem::path inputFolder{ m_activeProjectOptions.m_filePath.wc_str() };

    std::vector<std::filesystem::path> excludedPaths;
    for (const auto& currentFile : m_activeProjectOptions.m_excludedPaths)
        {
        excludedPaths.push_back(std::filesystem::path{ currentFile.wc_str() });
        }

    // input folder
    const std::vector<std::filesystem::path> filesToAnalyze = [&excludedPaths, &inputFolder]()
    {
        wxBusyInfo bi{ wxBusyInfoFlags{}.Text(_(L"Gathering files...")) };
        // paths being ignored
        const i18n_check::excluded_results excludedInfo =
            i18n_check::get_paths_files_to_exclude(inputFolder, excludedPaths);
        return i18n_check::get_files_to_analyze(inputFolder, excludedInfo.m_excludedPaths,
                                                excludedInfo.m_excludedFiles);
    }();

    const auto setSourceParserInfo = [this](auto& parser)
    {
        parser.set_style(static_cast<i18n_check::review_style>(m_activeProjectOptions.m_options));
        parser.log_messages_can_be_translatable(
            m_activeProjectOptions.m_logMessagesCanBeTranslated);
        parser.allow_translating_punctuation_only_strings(
            m_activeProjectOptions.m_allowTranslatingPunctuationOnlyStrings);
        parser.exceptions_should_be_translatable(
            m_activeProjectOptions.m_exceptionsShouldBeTranslatable);
        parser.set_min_words_for_classifying_unavailable_string(
            m_activeProjectOptions.m_minWordsForClassifyingUnavailableString);
        parser.set_min_cpp_version(m_activeProjectOptions.m_minCppVersion);
        for (const auto& pattern : m_activeProjectOptions.m_varsToIgnore)
            {
            try
                {
                parser.add_variable_name_pattern_to_ignore(std::wregex{ pattern.wc_str() });
                }
            catch (...)
                {
                m_logWindow->AppendText(wxString::Format(
                    _(L"\nInvalid regex pattern for ignored variable name: %s"), pattern));
                }
            }
    };

    i18n_check::cpp_i18n_review cpp;
    setSourceParserInfo(cpp);
    i18n_check::csharp_i18n_review csharp;
    setSourceParserInfo(csharp);

    i18n_check::rc_file_review rc;
    rc.set_style(static_cast<i18n_check::review_style>(m_activeProjectOptions.m_options));
    rc.allow_translating_punctuation_only_strings(
        m_activeProjectOptions.m_allowTranslatingPunctuationOnlyStrings);
    i18n_check::po_file_review po;
    po.set_style(static_cast<i18n_check::review_style>(m_activeProjectOptions.m_options));
    po.review_fuzzy_translations(m_activeProjectOptions.m_fuzzyTranslations);

    wxProgressDialog* progressDlg{ nullptr };

    i18n_check::batch_analyze analyzer(&cpp, &rc, &po, &csharp);

    if (m_activeProjectOptions.m_pseudoTranslationMethod !=
        i18n_check::pseudo_translation_method::none)
        {
        analyzer.pseudo_translate(
            filesToAnalyze, m_activeProjectOptions.m_pseudoTranslationMethod,
            m_activeProjectOptions.m_addPseudoTransBrackets,
            m_activeProjectOptions.m_widthPseudoIncrease, m_activeProjectOptions.m_pseudoTrack,
            [&progressDlg, this](const size_t totalFiles)
            {
                if (progressDlg != nullptr)
                    {
                    progressDlg->Destroy();
                    progressDlg = nullptr;
                    }
                if (totalFiles > 0)
                    {
                    progressDlg = new wxProgressDialog(
                        _(L"Pseudo-translating Files"), _(L"Pseudo-translating..."), totalFiles,
                        this,
                        wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
                    progressDlg->SetRange(totalFiles);
                    progressDlg->Centre();
                    }
            },
            [&progressDlg](const size_t currentFileIndex, const std::filesystem::path& file)
            {
                progressDlg->SetTitle(
                    wxString::Format(_(L"Pseudo-translating %s of %s..."),
                                     wxNumberFormatter::ToString(
                                         currentFileIndex, 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep),
                                     wxNumberFormatter::ToString(
                                         progressDlg->GetRange(), 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)));
#ifndef NDEBUG
                std::wcout << L"Pseudo-translating " << file << L"\n";
#endif
                if (!progressDlg->Update(currentFileIndex,
                                         file.empty() ?
                                             _(L"Processing...") :
                                             wxString::Format(_(L"Pseudo-translating %s..."),
#if CHECK_GCC_VERSION(12, 2, 1)
                                                              file.filename().wstring())))
#else
                                                              file.filename().string())))
#endif
                    {
                    progressDlg->Destroy();
                    progressDlg = nullptr;
                    return false;
                    }
                return true;
            });
        }

    analyzer.analyze(
        filesToAnalyze,
        [&progressDlg, this](const size_t totalFiles)
        {
            if (progressDlg != nullptr)
                {
                progressDlg->Destroy();
                progressDlg = nullptr;
                }
            if (totalFiles > 0)
                {
                progressDlg = new wxProgressDialog(
                    _(L"Analyzing Files"), _(L"Reviewing files for l10n/i18n issues..."),
                    totalFiles, this,
                    wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
                        wxPD_REMAINING_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
                progressDlg->Centre();
                }
        },
        [&progressDlg](const size_t currentFileIndex, const std::filesystem::path& file)
        {
            if (progressDlg != nullptr)
                {
                progressDlg->SetTitle(
                    wxString::Format(_(L"Processing %s of %s..."),
                                     wxNumberFormatter::ToString(
                                         currentFileIndex, 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep),
                                     wxNumberFormatter::ToString(
                                         progressDlg->GetRange(), 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)));
#ifndef NDEBUG
                std::wcout << L"Analyzing " << file << L"\n";
#endif
                if (!progressDlg->Update(currentFileIndex,
                                         file.empty() ?
                                             _(L"Processing...") :
                                             wxString::Format(_(L"Reviewing %s..."),
#if CHECK_GCC_VERSION(12, 2, 1)
                                                              file.filename().wstring())))
#else
                                                              file.filename().string())))
#endif
                    {
                    progressDlg->Destroy();
                    progressDlg = nullptr;
                    return false;
                    }
                }
            return true;
        });

    std::wstringstream report = analyzer.format_results(false);

    std::wstring currentLine;
    size_t readLines{ 0 };

    m_resultsModel->Clear();
    // read in the data
    while (std::getline(report, currentLine))
        {
        ++readLines;
        // skip header line
        if (readLines == 1)
            {
            continue;
            }
        wxStringTokenizer tokenizer(
            currentLine, L"\t",
            wxStringTokenizerMode(wxTOKEN_STRTOK | wxTOKEN_RET_EMPTY | wxTOKEN_RET_EMPTY_ALL));
        while (tokenizer.HasMoreTokens())
            {
            wxString fileName = tokenizer.GetNextToken();
            // strip quotes
            if (fileName.starts_with(L'\"'))
                {
                fileName.erase(0, 1);
                }
            if (fileName.ends_with(L'\"'))
                {
                fileName.erase(fileName.length() - 1);
                }
            const wxString lineNo = tokenizer.GetNextToken();
            const wxString columnNo = tokenizer.GetNextToken();
            const wxString warningValue = tokenizer.GetNextToken();
            const wxString explanation = tokenizer.GetNextToken();
            const wxString warningId = tokenizer.GetNextToken();

            int lnVal{ -1 }, cmVal{ -1 };
            lineNo.ToInt(&lnVal);
            columnNo.ToInt(&cmVal);

            m_resultsModel->AddRow(fileName, warningId, warningValue, explanation, lnVal, cmVal);
            }
        }

    // notify the control to refresh
    m_resultsModel->Cleared();

    ExpandAll();

    m_hasOpenProject = true;
    m_projectBar->EnableButton(wxID_SAVE, true);
    m_projectBar->EnableButton(wxID_REFRESH, true);
    m_projectBar->EnableButton(XRCID("ID_IGNORE_SELECTED"), true);
    m_editBar->EnableButton(XRCID("ID_EXPAND_ALL"), true);
    m_editBar->EnableButton(XRCID("ID_COLLAPSE_ALL"), true);

    m_logWindow->AppendText(analyzer.format_summary(false).str());
    m_logWindow->AppendText(L"\n");
    m_logWindow->AppendText(analyzer.get_log_report());
    m_logWindow->AppendText(L"\n");
    }

//------------------------------------------------------
bool I18NApp::OnInit()
    {
    if (!wxApp::OnInit())
        {
        return false;
        }

    SetAppName(L"Cuneiform");

    wxArtProvider::Push(new I18NArtProvider);

    // load the settings
    const wxString appSettingFolderPath = wxStandardPaths::Get().GetUserDataDir();
    if (!wxFileName::DirExists(appSettingFolderPath))
        {
        wxFileName::Mkdir(appSettingFolderPath);
        }
    m_optionsFilePath = appSettingFolderPath + wxFileName::GetPathSeparator() + L"settings.xml";
    if (wxFileName::FileExists(m_optionsFilePath))
        {
        m_defaultOptions.Load(m_optionsFilePath);
        }

    // set the locale (for number formatting, etc.) and load any translations
    wxUILocale::UseDefault();

    wxTranslations* const translations{ new wxTranslations{} };
    wxTranslations::Set(translations);
    if (!translations->AddCatalog(GetAppName()))
        {
        wxLogDebug(L"Could not find application's translation catalog.");
        }
    if (!translations->AddStdCatalog())
        {
        wxLogDebug(L"Could not find standard translation catalog.");
        }

#if wxCHECK_VERSION(3, 3, 0) && defined(__WXMSW__)
    MSWEnableDarkMode();
#endif

    // create the main application window
    I18NFrame* frame = new I18NFrame(GetAppName());
    frame->InitControls();
    frame->SetSize(frame->FromDIP(wxSize{ 1200, 1000 }));
    frame->CenterOnScreen();
    frame->Show(true);

    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, wxSize{ 32, 32 }));
    frame->SetIcon(appIcon);

    wxFontEnumerator fe;
    auto fontNames = fe.GetFacenames();
    for (const auto& fn : fontNames)
        {
        // don't ignore font names that might also be real words
        if (fn.CmpNoCase(_DT(L"Symbol")) == 0)
            {
            continue;
            }
        i18n_check::i18n_review::add_font_name_to_ignore(fn.wc_str());
        }

    return true;
    }

//------------------------------------------------------
int I18NApp::OnExit()
    {
    m_defaultOptions.Save(m_optionsFilePath);

    return 0;
    }
