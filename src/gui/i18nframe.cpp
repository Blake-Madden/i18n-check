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

#include "i18nframe.h"
#include "i18napp.h"

wxDECLARE_APP(I18NApp);

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
        m_projectBar->AddButton(
            wxID_NEW, _(L"New"),
            wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                .ConvertToImage());
        m_projectBar->AddButton(
            wxID_OPEN, _(L"Open"),
            wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                .ConvertToImage());
        m_projectBar->AddHybridButton(
            wxID_SAVE, _(L"Save"),
            wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                .ConvertToImage());
        m_projectBar->AddButton(
            wxID_REFRESH, _(L"Refresh"),
            wxArtProvider::GetBitmap(wxART_REFRESH, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                .ConvertToImage());
        m_projectBar->AddDropdownButton(
            XRCID("ID_IGNORE"), _(L"Ignore"),
            wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                .ConvertToImage());
        m_projectBar->EnableButton(wxID_SAVE, false);
        m_projectBar->EnableButton(wxID_REFRESH, false);
        m_projectBar->EnableButton(XRCID("ID_IGNORE"), false);

            {
            wxRibbonPanel* editPanel =
                new wxRibbonPanel(homePage, wxID_ANY, _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                  wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            m_editBar = new wxRibbonButtonBar(editPanel);

            m_editBar->AddButton(
                wxID_PASTE, _(L"Paste"),
                wxArtProvider::GetBitmap(wxART_PASTE, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());
            m_editBar->AddButton(
                wxID_CUT, _(L"Cut"),
                wxArtProvider::GetBitmap(wxART_CUT, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());
            m_editBar->AddButton(
                wxID_COPY, _(L"Copy"),
                wxArtProvider::GetBitmap(wxART_COPY, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());

            m_editBar->AddButton(
                wxID_UNDO, _(L"Undo"),
                wxArtProvider::GetBitmap(wxART_UNDO, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());
            m_editBar->AddButton(
                wxID_REDO, _(L"Redo"),
                wxArtProvider::GetBitmap(wxART_REDO, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());

            m_editBar->AddDropdownButton(
                XRCID("ID_INSERT"), _(L"Insert"),
                wxArtProvider::GetBitmap(L"ID_INSERT", wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());

            m_editBar->AddButton(
                wxID_SELECTALL, _(L"Select All"),
                wxArtProvider::GetBitmap(L"ID_SELECT_ALL", wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());

            EnableEditBar(false);
            }

            {
            wxRibbonPanel* toolsPanel =
                new wxRibbonPanel(homePage, wxID_ANY, _(L"Tools"), wxNullBitmap, wxDefaultPosition,
                                  wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* toolbar = new wxRibbonButtonBar(toolsPanel);
            toolbar->AddButton(
                XRCID("ID_SETTINGS"), _(L"Settings"),
                wxArtProvider::GetBitmap(L"ID_SETTINGS", wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());
            }

            {
            wxRibbonPage* helpPage = new wxRibbonPage(m_ribbon, wxID_ANY, _(L"Help"));
            wxRibbonPanel* helpPanel = new wxRibbonPanel(
                helpPage, wxID_ANY, _(L"General"), wxNullBitmap, wxDefaultPosition, wxDefaultSize,
                wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            wxRibbonButtonBar* helpbar = new wxRibbonButtonBar(helpPanel);
            helpbar->AddButton(
                wxID_HELP, _(L"Help"),
                wxArtProvider::GetBitmap(wxART_HELP, wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());
            helpbar->AddButton(
                wxID_ABOUT, _(L"About"),
                wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, FromDIP(wxSize{ 32, 32 }))
                    .ConvertToImage());
            }
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
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        m_ribbon->GetArtProvider()->SetColour(
            wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR,
            wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_TAB_ACTIVE_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_PANEL_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_TAB_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_ribbon->GetArtProvider()->SetColour(wxRIBBON_ART_TAB_HOVER_LABEL_COLOUR,
                                              wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        }

    m_ribbon->Realize();

    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(m_ribbon, wxSizerFlags{}.Expand());

    wxSplitterWindow* splitter = new wxSplitterWindow(this);

    m_resultsDataView = new wxDataViewCtrl(splitter, DATA_VIEW, wxDefaultPosition, wxDefaultSize,
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

    m_editor = new wxStyledTextCtrl(tabstrip, EDITOR_ID);
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
    m_editor->StyleSetBackground(
        ERROR_ANNOTATION_STYLE,
        wxSystemSettings::SelectLightDark(wxColour(244, 220, 220), wxColour(100, 100, 100)));
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
            wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_WINDOW));
        m_logWindow->SetForegroundColour(
            wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_BTNTEXT));
        }

    tabstrip->InsertPage(0, m_editor, _("Edit"));
    tabstrip->InsertPage(1, m_logWindow, _("Analysis Log"), false);
    tabstrip->SetMinSize(FromDIP(wxSize{ 200, 200 }));

    splitter->SplitHorizontally(m_resultsDataView, tabstrip,
                                FromDIP(-wxGetApp().m_defaultOptions.m_editorHeight));
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
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &I18NFrame::OnSaveMenu, this, wxID_SAVE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnRefresh, this, wxID_REFRESH);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &I18NFrame::OnIgnore, this, XRCID("ID_IGNORE"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &I18NFrame::OnInsert, this, XRCID("ID_INSERT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnSettings, this, XRCID("ID_SETTINGS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnHelp, this, wxID_HELP);
    Bind(wxEVT_CLOSE_WINDOW, &I18NFrame::OnClose, this);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEditButtonClicked, this, wxID_SELECTALL);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEditButtonClicked, this, wxID_UNDO);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEditButtonClicked, this, wxID_REDO);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEditButtonClicked, this, wxID_COPY);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEditButtonClicked, this, wxID_CUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEditButtonClicked, this, wxID_PASTE);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxDataViewItem selectedItem = m_resultsDataView->GetSelection();
            if (selectedItem.IsOk())
                {
                I18NResultsTreeModelNode* node =
                    reinterpret_cast<I18NResultsTreeModelNode*>(selectedItem.GetID());

                if (node != nullptr)
                    {
                    const wxString rowText =
                        node->m_warningId + L'\t' + node->m_issue + L'\t' +
                        std::to_wstring((node->m_line == -1) ? 0 : node->m_line) + L'\t' +
                        std::to_wstring((node->m_column == -1) ? 0 : node->m_column) + L'\t' +
                        node->m_explaination;
                    if (wxTheClipboard->Open())
                        {
                        if (rowText.length())
                            {
                            wxTheClipboard->Clear();
                            wxDataObjectComposite* obj = new wxDataObjectComposite();
                            obj->Add(new wxTextDataObject(rowText));
                            wxTheClipboard->SetData(obj);
                            }
                        wxTheClipboard->Close();
                        }
                    }
                }
        },
        wxID_COPY);
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
        wxEVT_MENU, [this](wxCommandEvent& event) { OnExportResults(event); },
        XRCID("ID_EXPORT_RESULTS"));
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& event) { OnInsertTranslatorComment(event); },
        XRCID("ID_INSERT_TRANSLATOR_COMMENT"));
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& event) { OnInsertEncodedUnicode(event); },
        XRCID("ID_CONVERT_TO_ENCODED_UNICODE"));
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& event) { OnInsertDTMacro(event); },
        XRCID("ID_INSERT_DT"));
    Bind(
        wxEVT_MENU, [this](wxCommandEvent& event) { OnInsertTGetTextMacro(event); },
        XRCID("ID_INSERT_GETTEXT"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnOpenSelectedFile(event);
        },
        XRCID("ID_OPEN_SELECTED"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnIgnoreSelectedFile(event);
        },
        XRCID("ID_IGNORE_SELECTED_FILE"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnIgnoreSelectedWarning(event);
        },
        XRCID("ID_IGNORE_SELECTED_WARNING"));
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
    Bind(wxEVT_CHILD_FOCUS,
         [this](wxChildFocusEvent& evt)
         {
             if (evt.GetWindow()->GetId() == EDITOR_ID)
                 {
                 I18NResultsTreeModelNode* node = reinterpret_cast<I18NResultsTreeModelNode*>(
                     m_resultsDataView->GetSelection().GetID());
                 if (node != nullptr)
                     {
                     // not selecting file name root node and a source file
                     if (node->m_fileName != node->m_warningId &&
                         i18n_check::is_source_file(node->m_fileName.wc_string()))
                         {
                         EnableEditBar(true);
                         }
                     }
                 }
             else if (evt.GetWindow()->GetId() == DATA_VIEW ||
                      (evt.GetWindow()->GetParent() != nullptr &&
                       evt.GetWindow()->GetParent()->GetId() == DATA_VIEW))
                 {
                 EnableEditBar(false);
                 }
             evt.Skip();
         });
    Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
         [this](wxDataViewEvent& event)
         {
             if (event.GetItem() == m_resultsModel->GetRoot())
                 {
                 return;
                 }

             I18NResultsTreeModelNode* node =
                 reinterpret_cast<I18NResultsTreeModelNode*>(event.GetItem().GetID());
             if (node != nullptr)
                 {
                 wxMenu menu;
                 wxMenuItem* menuItem =
                     new wxMenuItem(&menu, XRCID("ID_OPEN_SELECTED"),
                                    wxString::Format(_(L"Open \"%s\""),
                                                     wxFileName{ node->m_fileName }.GetFullName()));
                 menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER,
                                                              FromDIP(wxSize{ 16, 16 })));
                 menu.Append(menuItem);
                 menu.AppendSeparator();

                 menuItem = new wxMenuItem(&menu, wxID_COPY, _(L"Copy"));
                 menuItem->SetBitmap(
                     wxArtProvider::GetBitmap(wxART_COPY, wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
                 menu.Append(menuItem);
                 menu.AppendSeparator();

                 menuItem =
                     new wxMenuItem(&menu, XRCID("ID_IGNORE_SELECTED_FILE"),
                                    wxString::Format(_(L"Ignore \"%s\""),
                                                     wxFileName{ node->m_fileName }.GetFullName()));
                 menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER,
                                                              FromDIP(wxSize{ 16, 16 })));
                 menu.Append(menuItem);

                 if (node->m_warningId != node->m_fileName)
                     {
                     menuItem = new wxMenuItem(
                         &menu, XRCID("ID_IGNORE_SELECTED_WARNING"),
                         wxString::Format(_(L"Ignore '%s' Warnings"), node->m_warningId));
                     menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER,
                                                                  FromDIP(wxSize{ 16, 16 })));
                     menu.Append(menuItem);
                     }

                 m_resultsDataView->PopupMenu(&menu);
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
void I18NFrame::OnEditButtonClicked(wxRibbonButtonBarEvent& event)
    {
    if (event.GetId() == wxID_SELECTALL)
        {
        m_editor->SelectAll();
        }
    else if (event.GetId() == wxID_UNDO)
        {
        m_editor->Undo();
        }
    else if (event.GetId() == wxID_REDO)
        {
        m_editor->Redo();
        }
    else if (event.GetId() == wxID_COPY)
        {
        m_editor->Copy();
        }
    else if (event.GetId() == wxID_CUT)
        {
        m_editor->Cut();
        }
    else if (event.GetId() == wxID_PASTE)
        {
        m_editor->Paste();
        }
    }

//------------------------------------------------------
void I18NFrame::OnIgnoreSelectedWarning([[maybe_unused]] wxCommandEvent&)
    {
    wxDataViewItem selectedItem = m_resultsDataView->GetSelection();

    if (selectedItem.IsOk())
        {
        I18NResultsTreeModelNode* node =
            reinterpret_cast<I18NResultsTreeModelNode*>(selectedItem.GetID());

        if (node != nullptr)
            {
            // file node selected, so no warning is selected
            if (node->m_fileName == node->m_warningId)
                {
                return;
                }

            SaveSourceFileIfNeeded();
            m_activeSourceFile.clear();
            m_editor->SetText(wxString{});

            // excludes a flag if the provided value matches the node's warning
            const auto excludeFlag = [this, &node](const wxString& value, const auto flag)
            {
                if (node->m_warningId == value)
                    {
                    m_activeProjectOptions.m_options = m_activeProjectOptions.m_options & ~flag;
                    }
            };

            excludeFlag(L"[notL10NAvailable]",
                        i18n_check::review_style::check_not_available_for_l10n);
            excludeFlag(L"[suspectL10NString]", i18n_check::review_style::check_l10n_strings);
            excludeFlag(L"[suspectL10NUsage]",
                        i18n_check::review_style::check_suspect_l10n_string_usage);
            excludeFlag(L"[printfMismatch]",
                        i18n_check::review_style::check_mismatching_printf_commands);
            excludeFlag(L"[acceleratorMismatch]", i18n_check::review_style::check_accelerators);
            excludeFlag(L"[transInconsistency]", i18n_check::review_style::check_consistency);
            excludeFlag(L"[L10NStringNeedsContext]",
                        i18n_check::review_style::check_needing_context);
            excludeFlag(L"[urlInL10NString]", i18n_check::review_style::check_l10n_contains_url);
            excludeFlag(L"[spacesAroundL10NString]",
                        i18n_check::review_style::check_l10n_has_surrounding_spaces);
            excludeFlag(L"[deprecatedMacro]", i18n_check::review_style::check_deprecated_macros);
            excludeFlag(L"[nonUTF8File]", i18n_check::review_style::check_utf8_encoded);
            excludeFlag(L"[UTF8FileWithBOM]", i18n_check::review_style::check_utf8_with_signature);
            excludeFlag(L"[unencodedExtASCII]",
                        i18n_check::review_style::check_unencoded_ext_ascii);
            excludeFlag(L"[printfSingleNumber]",
                        i18n_check::review_style::check_printf_single_number);
            excludeFlag(L"[numberAssignedToId]",
                        i18n_check::review_style::check_number_assigned_to_id);
            excludeFlag(L"[dupValAssignedToIds]",
                        i18n_check::review_style::check_duplicate_value_assigned_to_ids);
            excludeFlag(L"[malformedString]", i18n_check::review_style::check_malformed_strings);
            excludeFlag(L"[trailingSpaces]", i18n_check::review_style::check_trailing_spaces);
            excludeFlag(L"[fontIssue]", i18n_check::review_style::check_fonts);
            excludeFlag(L"[tabs]", i18n_check::review_style::check_tabs);
            excludeFlag(L"[wideLine]", i18n_check::review_style::check_line_width);
            excludeFlag(L"[commentMissingSpace]",
                        i18n_check::review_style::check_space_after_comment);

            m_resultsModel->DeleteWarning(node->m_warningId);

            m_projectDirty = true;

            SetTitleDirty();
            }
        }
    }

//------------------------------------------------------
void I18NFrame::OnOpenSelectedFile([[maybe_unused]] wxCommandEvent&)
    {
    wxDataViewItem selectedItem = m_resultsDataView->GetSelection();

    if (selectedItem.IsOk())
        {
        I18NResultsTreeModelNode* node =
            reinterpret_cast<I18NResultsTreeModelNode*>(selectedItem.GetID());
        if (node != nullptr)
            {
            wxLaunchDefaultApplication(node->m_fileName);
            }
        }
    }

//------------------------------------------------------
void I18NFrame::OnSaveMenu(wxRibbonButtonBarEvent& event)
    {
    wxMenu menu;
    wxMenuItem* menuItem = new wxMenuItem(&menu, wxID_SAVE, _(L"Save Project..."));
    menuItem->SetBitmap(
        wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
    menu.Append(menuItem);

    menuItem = new wxMenuItem(&menu, XRCID("ID_EXPORT_RESULTS"), _(L"Export Results..."));
    menuItem->SetBitmap(
        wxArtProvider::GetBitmap(L"ID_CSV", wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
    menu.Append(menuItem);

    event.PopupMenu(&menu);
    }

//------------------------------------------------------
void I18NFrame::OnInsert(wxRibbonButtonBarEvent& event)
    {
    wxMenu menu;
    wxMenuItem* menuItem =
        new wxMenuItem(&menu, XRCID("ID_INSERT_TRANSLATOR_COMMENT"), _(L"Translator Comment..."));
    menuItem->SetBitmap(wxArtProvider::GetBitmap(L"ID_INSERT_TRANSLATOR_COMMENT", wxART_OTHER,
                                                 FromDIP(wxSize{ 16, 16 })));
    menu.Append(menuItem);

    menuItem = new wxMenuItem(&menu, XRCID("ID_CONVERT_TO_ENCODED_UNICODE"),
                              _(L"Encode Extended ASCII Characters..."));
    menuItem->SetBitmap(wxArtProvider::GetBitmap(L"ID_CONVERT_TO_ENCODED_UNICODE", wxART_OTHER,
                                                 FromDIP(wxSize{ 16, 16 })));
    menu.Append(menuItem);
    menu.AppendSeparator();

    menuItem = new wxMenuItem(&menu, XRCID("ID_INSERT_GETTEXT"), _(L"Mark for Translation..."));
    menuItem->SetBitmap(
        wxArtProvider::GetBitmap(L"ID_INSERT_GETTEXT", wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
    menu.Append(menuItem);

    menuItem = new wxMenuItem(&menu, XRCID("ID_INSERT_DT"), _(L"Mark as Non-translatable..."));
    menuItem->SetBitmap(
        wxArtProvider::GetBitmap(L"ID_INSERT_DT", wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
    menu.Append(menuItem);

    event.PopupMenu(&menu);
    }

//------------------------------------------------------
void I18NFrame::OnIgnore(wxRibbonButtonBarEvent& event)
    {
    wxDataViewItem selectedItem = m_resultsDataView->GetSelection();
    if (selectedItem == m_resultsModel->GetRoot())
        {
        return;
        }

    if (selectedItem.IsOk())
        {
        I18NResultsTreeModelNode* node =
            reinterpret_cast<I18NResultsTreeModelNode*>(selectedItem.GetID());
        if (node != nullptr)
            {
            wxMenu menu;
            wxMenuItem* menuItem =
                new wxMenuItem(&menu, XRCID("ID_IGNORE_SELECTED_FILE"),
                               wxString::Format(_(L"Ignore \"%s\""),
                                                wxFileName{ node->m_fileName }.GetFullName()));
            menuItem->SetBitmap(
                wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
            menu.Append(menuItem);

            if (node->m_warningId != node->m_fileName)
                {
                menuItem =
                    new wxMenuItem(&menu, XRCID("ID_IGNORE_SELECTED_WARNING"),
                                   wxString::Format(_(L"Ignore '%s' Warnings"), node->m_warningId));
                menuItem->SetBitmap(
                    wxArtProvider::GetBitmap(wxART_DELETE, wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
                menu.Append(menuItem);
                }

            event.PopupMenu(&menu);
            }
        }
    }

//------------------------------------------------------
void I18NFrame::OnIgnoreSelectedFile([[maybe_unused]] wxCommandEvent&)
    {
    wxDataViewItem selectedItem = m_resultsDataView->GetSelection();
    if (selectedItem == m_resultsModel->GetRoot())
        {
        return;
        }

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

            m_activeProjectOptions.m_excludedPaths.push_back(node->m_fileName);

            m_resultsModel->Delete(selectedItem);

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
    const wxString docPath = []()
    {
        if (wxFile::Exists(wxStandardPaths::Get().GetResourcesDir() +
                           wxFileName::GetPathSeparator() + L"cuneiform.pdf"))
            {
            return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() +
                   L"cuneiform.pdf";
            }
        return wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath() +
               wxFileName::GetPathSeparator() + L"cuneiform.pdf";
    }();
    if (wxFile::Exists(docPath))
        {
        wxLaunchDefaultApplication(docPath);
        }
    }

//------------------------------------------------------
void I18NFrame::OnAbout([[maybe_unused]] wxCommandEvent&)
    {
    wxDateTime buildDate;
    buildDate.ParseDate(__DATE__);

    wxAboutDialogInfo aboutInfo;
    aboutInfo.AddDeveloper(L"Blake Madden");
    aboutInfo.SetLicence(_DT(LR"(Eclipse Public License - v 2.0)"));
    aboutInfo.SetCopyright(
        wxString::Format(_(L"Copyright \U000000A92021-%d %s. All rights reserved."),
                         buildDate.GetYear(), L"Blake Madden"));
    wxIcon appIcon;
    appIcon.CopyFromBitmap(
        wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, wxWindow::FromDIP(wxSize{ 32, 32 })));
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
        CopyProjectOptionsToGlobalOptions();

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
    CopyProjectOptionsToGlobalOptions();

    SetTitle(wxString::Format(
        /* TRANSLATORS: %s is app name */ _(L"%s - Untitled"), wxGetApp().GetAppName()));

    Process();
    }

//------------------------------------------------------
void I18NFrame::CopyProjectOptionsToGlobalOptions()
    {
    // Insert any new var patterns to ignore that user entered from a project settings dialog
    // into the global options.
    for (const auto& var : m_activeProjectOptions.m_varsToIgnore)
        {
        if (std::find_if(wxGetApp().m_defaultOptions.m_varsToIgnore.cbegin(),
                         wxGetApp().m_defaultOptions.m_varsToIgnore.cend(),
                         [&var](const auto& val) { return val == var; }) ==
            wxGetApp().m_defaultOptions.m_varsToIgnore.cend())
            {
            wxGetApp().m_defaultOptions.m_varsToIgnore.push_back(var);
            }
        }
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
void I18NFrame::OnInsertTranslatorComment([[maybe_unused]] wxCommandEvent&)
    {
    wxTextEntryDialog dialog(
        this, _("Enter an explanation for a string that provides context for the translators:"),
        _("Translator Comment"), wxString{}, wxTextEntryDialogStyle | wxTE_MULTILINE,
        wxDefaultPosition, wxWindow::FromDIP(wxSize{ 175, 250 }));
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_editor->InsertText(m_editor->GetCurrentPos(),
                         _DT(L"/* TRANSLATORS: ") + dialog.GetValue() + L" */");
    }

//------------------------------------------------------
void I18NFrame::OnInsertEncodedUnicode([[maybe_unused]] wxCommandEvent&)
    {
    const std::wstring selText{ m_editor->GetSelectedText().wc_string() };
    if (selText.empty())
        {
        wxMessageBox(_(L"No selection found. Please select a string in the editor to encode."),
                     _(L"No Selection"));
        return;
        }

    std::wstringstream encoded;
    bool encodingRequired{ false };
    for (const auto& ch : selText)
        {
        if (ch > 127)
            {
            encoded << LR"(\U)" << std::setfill(L'0') << std::setw(8) << std::uppercase << std::hex
                    << static_cast<int>(ch);
            encodingRequired = true;
            }
        else
            {
            encoded << ch;
            }
        }

    if (!encodingRequired)
        {
        wxMessageBox(
            _(L"No extended ASCII characters found in the selection that need to be encoded."),
            _(L"Encoding Not Required"));
        return;
        }

    if (wxMessageBox(wxString::Format(
                         _(L"Replace the selected text:\n\n'%s'\n\nwith the following?\n\n'%s'"),
                         selText, encoded.str()),
                     _(L"Encode Extended ASCII Characters"),
                     wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
        {
        m_editor->ReplaceSelection(encoded.str());
        }
    }

//------------------------------------------------------
void I18NFrame::OnInsertTGetTextMacro([[maybe_unused]] wxCommandEvent&)
    {
    const wxString selText{ m_editor->GetSelectedText() };
    if (selText.empty())
        {
        wxMessageBox(_(L"No selection found. Please select a quoted string in the editor."),
                     _(L"No Selection"));
        return;
        }
    else if (!selText.starts_with(L'"') || !selText.ends_with(L'"'))
        {
        wxMessageBox(_(L"Please select a quoted string in the editor to wrap within a _() macro."),
                     _(L"No Quote Selected"));
        return;
        }

    m_editor->ReplaceSelection(L"_(" + selText + ")");
    }

//------------------------------------------------------
void I18NFrame::OnInsertDTMacro([[maybe_unused]] wxCommandEvent&)
    {
    const wxString selText{ m_editor->GetSelectedText() };
    if (selText.empty())
        {
        wxMessageBox(_(L"No selection found. Please select a quoted string in the editor."),
                     _(L"No Selection"));
        return;
        }
    else if (!selText.starts_with(L'"') || !selText.ends_with(L'"'))
        {
        wxMessageBox(
            _(L"Please select a quoted string in the editor to wrap within a _DT() macro."),
            _(L"No Quote Selected"));
        return;
        }

    m_editor->ReplaceSelection(L"_DT(" + selText + ")");
    }

//------------------------------------------------------
void I18NFrame::OnExportResults([[maybe_unused]] wxCommandEvent&)
    {
    const wxFileName projectName{ m_activeProjectOptions.m_filePath };
    const wxString lastFolder =
        projectName.GetName().empty() ? wxString{ _(L"Results") } : projectName.GetName();

    wxFileDialog dialog(nullptr, _(L"Export Results"), wxString{}, lastFolder + L".csv",
                        _(L"Comma Separated Values (*.csv)|*.csv|Tab-delimited Text (*.txt)|*.txt"),
                        wxFD_SAVE | wxFD_PREVIEW | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxString outText{ m_activeResults };
    if (dialog.GetFilterIndex() == 0)
        {
        // convert to CSV
        outText.Replace(L"\t", L",", true);
        }

    wxFile outFile(dialog.GetPath(), wxFile::write);
    if (outFile.IsOpened())
        {
        if (!outFile.Write(outText))
            {
            wxMessageBox(_(L"Unable to export results."), _(L"Export"));
            }
        }
    else
        {
        wxMessageBox(_(L"Unable to export results."), _(L"Export"));
        }
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
            const std::wstring fileText = m_editor->GetText().wc_string();
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

    std::filesystem::path inputFolder{ m_activeProjectOptions.m_filePath.wc_string() };

    std::vector<std::filesystem::path> excludedPaths;
    for (const auto& currentFile : m_activeProjectOptions.m_excludedPaths)
        {
        excludedPaths.push_back(std::filesystem::path{ currentFile.wc_string() });
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
                parser.add_variable_name_pattern_to_ignore(std::wregex{ pattern.wc_string() });
                }
            catch (...)
                {
                m_logWindow->AppendText(wxString::Format(
                    _(L"\nInvalid regex pattern for ignored variable name: %s"), pattern));
                }
            }
    };

    i18n_check::cpp_i18n_review cpp(m_activeProjectOptions.m_verbose);
    setSourceParserInfo(cpp);
    i18n_check::csharp_i18n_review csharp(m_activeProjectOptions.m_verbose);
    setSourceParserInfo(csharp);

    i18n_check::rc_file_review rc(m_activeProjectOptions.m_verbose);
    rc.set_style(static_cast<i18n_check::review_style>(m_activeProjectOptions.m_options));
    rc.allow_translating_punctuation_only_strings(
        m_activeProjectOptions.m_allowTranslatingPunctuationOnlyStrings);
    i18n_check::po_file_review po(m_activeProjectOptions.m_verbose);
    po.set_style(static_cast<i18n_check::review_style>(m_activeProjectOptions.m_options));
    po.review_fuzzy_translations(m_activeProjectOptions.m_fuzzyTranslations);

    i18n_check::batch_analyze analyzer(&cpp, &rc, &po, &csharp);

    if (m_activeProjectOptions.m_pseudoTranslationMethod !=
        i18n_check::pseudo_translation_method::none)
        {
        wxProgressDialog progressDlg = wxProgressDialog(
            _(L"Pseudo-translating Files"), _(L"Pseudo-translating..."), filesToAnalyze.size(),
            this,
            wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
                wxPD_REMAINING_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
        wxAppProgressIndicator progress(this);

        analyzer.pseudo_translate(
            filesToAnalyze, m_activeProjectOptions.m_pseudoTranslationMethod,
            m_activeProjectOptions.m_addPseudoTransBrackets,
            m_activeProjectOptions.m_widthPseudoIncrease, m_activeProjectOptions.m_pseudoTrack,
            [&progressDlg, &progress](const size_t totalFiles)
            {
                progressDlg.SetRange(totalFiles);
                progressDlg.Update(0);

                progress.SetRange(totalFiles);
                progress.SetValue(0);
            },
            [&progressDlg, &progress](const size_t currentFileIndex,
                                      const std::filesystem::path& file)
            {
                progressDlg.SetTitle(
                    wxString::Format(_(L"Pseudo-translating %s of %s..."),
                                     wxNumberFormatter::ToString(
                                         currentFileIndex, 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep),
                                     wxNumberFormatter::ToString(
                                         progressDlg.GetRange(), 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)));
#ifndef NDEBUG
                std::wcout << L"Pseudo-translating " << file << L"\n";
#endif
                if (!progressDlg.Update(currentFileIndex,
                                        file.empty() ?
                                            _(L"Processing...") :
                                            wxString::Format(_(L"Pseudo-translating %s..."),
#if CHECK_GCC_VERSION(12, 2, 1)
                                                             file.filename().wstring())))
#else
                                                             file.filename().string())))
#endif
                    {
                    return false;
                    }
                progress.SetValue(currentFileIndex);
                return true;
            });
        }

    wxProgressDialog progressDlg =
        wxProgressDialog(_(L"Analyzing Files"), _(L"Reviewing files for l10n/i18n issues..."),
                         filesToAnalyze.size(), this,
                         wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
                             wxPD_REMAINING_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
    wxAppProgressIndicator progress(this);

    analyzer.analyze(
        filesToAnalyze,
        [&progressDlg, &progress](const size_t totalFiles)
        {
            // We set an extra step here so that the dialog doesn't autohide.
            // This is necessary because we need to reuse this dialog for when it analyzes
            // translation catalog entries, and if it autohides then it won't be visible then.
            // This dialog will close when it goes out of scope (autohiding will take over then).
            progressDlg.SetRange(totalFiles + 1);
            progressDlg.Update(0);

            progress.SetRange(totalFiles);
            progress.SetValue(0);
        },
        [&progressDlg, &progress](const size_t currentFileIndex, const std::filesystem::path& file)
        {
            progressDlg.Show();
            progressDlg.SetTitle(wxString::Format(
                _(L"Processing %s of %s..."),
                wxNumberFormatter::ToString(currentFileIndex, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxNumberFormatter::ToString(progressDlg.GetRange(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)));
#ifndef NDEBUG
            std::wcout << L"Analyzing " << file << L"\n";
#endif
            if (!progressDlg.Update(currentFileIndex,
                                    file.empty() ? _(L"Processing...") :
                                                   wxString::Format(_(L"Reviewing %s..."),
#if CHECK_GCC_VERSION(12, 2, 1)
                                                                    file.filename().wstring())))
#else
                                                                    file.filename().string())))
#endif
                {
                return false;
                }
            progress.SetValue(currentFileIndex);
            return true;
        });

    std::wstringstream report = analyzer.format_results(cpp.is_verbose());
    m_activeResults = report.str();

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
            // normalize paths, as filesystem::path may store Windows separators as "\\"
            fileName.Replace(LR"(\\)", LR"(\)", true);
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

    if (m_resultsDataView->GetColumn(0) != nullptr)
        {
        m_resultsDataView->GetColumn(0)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
        }
    if (m_resultsDataView->GetColumn(2) != nullptr)
        {
        m_resultsDataView->GetColumn(2)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
        }
    if (m_resultsDataView->GetColumn(3) != nullptr)
        {
        m_resultsDataView->GetColumn(3)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
        }

    m_hasOpenProject = true;
    m_projectBar->EnableButton(wxID_SAVE, true);
    m_projectBar->EnableButton(wxID_REFRESH, true);
    m_projectBar->EnableButton(XRCID("ID_IGNORE"), true);

    m_logWindow->AppendText(analyzer.format_summary(wxGetApp().m_defaultOptions.m_verbose).str());
    m_logWindow->AppendText(L"\n");
    m_logWindow->AppendText(analyzer.get_log_report());
    m_logWindow->AppendText(L"\n");
    }

//------------------------------------------------------
void I18NFrame::OnClose(wxCloseEvent& event)
    {
    wxGetApp().m_defaultOptions.m_windowMaximized = IsMaximized();
    wxGetApp().m_defaultOptions.m_windowSize = ToDIP(GetSize());
    wxGetApp().m_defaultOptions.m_editorHeight = ToDIP(m_editor->GetSize().GetHeight());
    event.Skip();
    }
