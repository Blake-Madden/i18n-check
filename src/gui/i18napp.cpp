//////////////////////////////////////////////////////////////////////////////
// Name:        i18napp.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "i18napp.h"

wxIMPLEMENT_APP(I18NApp);

I18NArtProvider::I18NArtProvider()
    {
    // cppcheck-suppress useInitializationList
    m_idFileMap = { { wxART_FILE_OPEN, L"images/file-open.svg" },
                    { wxART_FILE_SAVE, L"images/file-save.svg" },
                    { wxART_NEW, L"images/document.svg" },
                    { wxART_EDIT, L"images/reload.svg" },
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
            wxID_EDIT, _(L"Refresh"),
            wxArtProvider::GetBitmap(wxART_EDIT, wxART_OTHER, wxSize{ 32, 32 }));
        m_projectBar->EnableButton(wxID_SAVE, false);
        m_projectBar->EnableButton(wxID_EDIT, false);

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
            new wxRibbonPanel(homePage, wxID_ANY, _(L"Help"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar* toolbar = new wxRibbonButtonBar(helpPanel);
        toolbar->AddButton(wxID_ABOUT, _(L"About"),
                           wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, wxSize{ 32, 32 }));
        }

    m_ribbon->Realize();

    wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(m_ribbon, wxSizerFlags().Expand());

    wxSplitterWindow* splitter = new wxSplitterWindow(this);

    m_resultsDataView = new wxDataViewCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                           wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_SINGLE);

    m_resultsModel = new I18NResultsTreeModel;

    m_resultsDataView->AssociateModel(m_resultsModel.get());

    // Warning ID
    wxDataViewTextRenderer* tr = new wxDataViewTextRenderer(L"string", wxDATAVIEW_CELL_INERT);
    m_resultsDataView->AppendColumn(
        new wxDataViewColumn(_(L"Warning ID"), tr, 0, FromDIP(200), wxALIGN_LEFT,
                             wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE));

    // Value
    tr = new wxDataViewTextRenderer(L"string", wxDATAVIEW_CELL_INERT);
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
    tr = new wxDataViewTextRenderer(L"string", wxDATAVIEW_CELL_INERT);
    m_resultsDataView->AppendColumn(
        new wxDataViewColumn(_(L"Summary"), tr, 4, FromDIP(200), wxALIGN_LEFT,
                             wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE));

    m_editor = new wxStyledTextCtrl(splitter);
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
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_DOTDOTDOT, *wxBLACK, *wxBLACK);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, *wxBLACK, *wxBLACK);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY, *wxBLACK, *wxBLACK);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_DOTDOTDOT, *wxBLACK, *wxWHITE);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, *wxBLACK, *wxWHITE);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY, *wxBLACK, *wxBLACK);
    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY, *wxBLACK, *wxBLACK);
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

    const wxColor grey(100, 100, 100);
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
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, L"WHITE");

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, "WHITE");

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDERMIDTAIL, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDERMIDTAIL, grey);

    m_editor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
    m_editor->MarkerSetForeground(wxSTC_MARKNUM_FOLDERTAIL, grey);
    m_editor->MarkerSetBackground(wxSTC_MARKNUM_FOLDERTAIL, grey);

    m_editor->SetWrapMode(wxSTC_WRAP_NONE);

    m_editor->StyleSetForeground(wxSTC_C_STRING, wxColour(150, 0, 0));
    m_editor->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(165, 105, 0));
    m_editor->StyleSetForeground(wxSTC_C_IDENTIFIER, wxColour(40, 0, 60));
    m_editor->StyleSetForeground(wxSTC_C_NUMBER, wxColour(0, 150, 0));
    m_editor->StyleSetForeground(wxSTC_C_CHARACTER, wxColour(150, 0, 0));
    m_editor->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 150));
    m_editor->StyleSetForeground(wxSTC_C_WORD2, wxColour(0, 150, 0));
    m_editor->StyleSetForeground(wxSTC_C_COMMENT, wxColour(150, 150, 150));
    m_editor->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(150, 150, 150));
    m_editor->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(150, 150, 150));
    m_editor->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD, wxColour(0, 0, 200));
    m_editor->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(0, 0, 200));
    m_editor->StyleSetBold(wxSTC_C_WORD, true);
    m_editor->StyleSetBold(wxSTC_C_WORD2, true);
    m_editor->StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD, true);

    m_editor->CallTipUseStyle(40);

    splitter->SplitHorizontally(m_resultsDataView, m_editor, FromDIP(-300));
    mainSizer->Add(splitter, wxSizerFlags(1).Expand());

    SetSizer(mainSizer);

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
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEdit, this, wxID_EDIT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnExpandAll, this, XRCID("ID_EXPAND_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnCollapseAll, this, XRCID("ID_COLLAPSE_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnAbout, this, wxID_ABOUT);
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
            OnEdit(event);
        },
        wxID_EDIT);
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
                     m_editor->GotoLine(node->m_line + 1);
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
void I18NFrame::OnAbout([[maybe_unused]] wxCommandEvent&)
    {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.AddDeveloper(L"Blake Madden");
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, wxSize{ 32, 32 }));
    aboutInfo.SetIcon(appIcon);
    wxAboutBox(aboutInfo, this);
    }

//------------------------------------------------------
void I18NFrame::OnEdit([[maybe_unused]] wxCommandEvent&)
    {
    NewProjectDialog projDlg(this);
    projDlg.SetOptions(static_cast<i18n_check::review_style>(m_options));
    projDlg.SetPath(m_filePath);
    projDlg.SetExcludedPath(m_excludedPaths);
    projDlg.UseFuzzyTranslations(m_fuzzyTranslations);
    projDlg.LogMessagesCanBeTranslated(m_logMessagesCanBeTranslated);
    projDlg.AllowTranslatingPunctuationOnlyStrings(m_allowTranslatingPunctuationOnlyStrings);
    projDlg.ExceptionsShouldBeTranslatable(m_exceptionsShouldBeTranslatable);
    projDlg.MinWordsForClassifyingUnavailableString(m_minWordsForClassifyingUnavailableString);
    projDlg.MinCppVersion(m_minCppVersion);

    if (projDlg.ShowModal() == wxID_OK)
        {
        m_options = projDlg.GetOptions();
        m_filePath = projDlg.GetPath();
        m_excludedPaths = projDlg.GetExcludedPath();
        m_fuzzyTranslations = projDlg.UseFuzzyTranslations();
        m_logMessagesCanBeTranslated = projDlg.LogMessagesCanBeTranslated();
        m_allowTranslatingPunctuationOnlyStrings = projDlg.AllowTranslatingPunctuationOnlyStrings();
        m_exceptionsShouldBeTranslatable = projDlg.ExceptionsShouldBeTranslatable();
        m_minWordsForClassifyingUnavailableString =
            projDlg.MinWordsForClassifyingUnavailableString();
        m_minCppVersion = projDlg.MinCppVersion();

        Process();

        m_projectDirty = true;
        }
    }

//------------------------------------------------------
void I18NFrame::OnNew([[maybe_unused]] wxCommandEvent&)
    {
    SaveProjectIfNeeded();

    NewProjectDialog projDlg(this);
    projDlg.SetOptions(static_cast<i18n_check::review_style>(m_options));
    projDlg.UseFuzzyTranslations(m_fuzzyTranslations);
    projDlg.LogMessagesCanBeTranslated(m_logMessagesCanBeTranslated);
    projDlg.AllowTranslatingPunctuationOnlyStrings(m_allowTranslatingPunctuationOnlyStrings);
    projDlg.ExceptionsShouldBeTranslatable(m_exceptionsShouldBeTranslatable);
    projDlg.MinWordsForClassifyingUnavailableString(m_minWordsForClassifyingUnavailableString);
    projDlg.MinCppVersion(m_minCppVersion);

    if (projDlg.ShowModal() == wxID_CANCEL)
        {
        return;
        }

    m_options = projDlg.GetOptions();
    m_filePath = projDlg.GetPath();
    m_excludedPaths = projDlg.GetExcludedPath();
    m_fuzzyTranslations = projDlg.UseFuzzyTranslations();
    m_logMessagesCanBeTranslated = projDlg.LogMessagesCanBeTranslated();
    m_allowTranslatingPunctuationOnlyStrings = projDlg.AllowTranslatingPunctuationOnlyStrings();
    m_exceptionsShouldBeTranslatable = projDlg.ExceptionsShouldBeTranslatable();
    m_minWordsForClassifyingUnavailableString = projDlg.MinWordsForClassifyingUnavailableString();
    m_minCppVersion = projDlg.MinCppVersion();

    Process();
    }

//------------------------------------------------------
void I18NFrame::OnOpen([[maybe_unused]] wxCommandEvent&)
    {
    SaveProjectIfNeeded();

    wxFileDialog dialog(this, _(L"Select Project to Open"), wxString{}, wxString{},
                        _(L"i18n Project Files (*.xml)|*.xml"),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxXmlDocument xmlDoc;
    if (!xmlDoc.Load(dialog.GetPath()))
        {
        wxMessageBox(_(L"Error loading project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    if (xmlDoc.GetRoot()->GetName() != "i18n-check-settings")
        {
        wxMessageBox(_(L"Invalid project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    wxXmlNode* child = xmlDoc.GetRoot()->GetChildren();
    while (child != nullptr)
        {
        if (child->GetName() == L"path")
            {
            m_filePath = child->GetNodeContent();
            }
        else if (child->GetName() == L"excluded-paths")
            {
            m_excludedPaths = child->GetNodeContent();
            }
        else if (child->GetName() == L"checks")
            {
            const wxString intVal = child->GetNodeContent();
            intVal.ToInt(&m_options);
            }
        else if (child->GetName() == L"fuzzy-translations")
            {
            m_fuzzyTranslations = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"log-messages-can-be-translated")
            {
            m_logMessagesCanBeTranslated = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"allow-translating-punctuation-only-strings")
            {
            m_allowTranslatingPunctuationOnlyStrings = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"exceptions-should-be-translatable")
            {
            m_exceptionsShouldBeTranslatable = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"min-words-for-classifying-unavailable-string")
            {
            const wxString intVal = child->GetNodeContent();
            intVal.ToInt(&m_minWordsForClassifyingUnavailableString);
            }
        else if (child->GetName() == L"min-cpp-version")
            {
            const wxString intVal = child->GetNodeContent();
            intVal.ToInt(&m_minCppVersion);
            }

        child = child->GetNext();
        }

    Process();
    }

//------------------------------------------------------
void I18NFrame::OnSave([[maybe_unused]] wxCommandEvent&)
    {
    const wxFileName projectName{ m_filePath };
    const wxString lastFolder = projectName.GetDirs().empty() ?
                                    wxString{ L"project" } :
                                    projectName.GetDirs()[projectName.GetDirs().size() - 1];
    wxFileDialog dialog(nullptr, _(L"Save Project"), wxString{}, lastFolder + L".xml",
                        _(L"i18n Project Files (*.xml)|*.xml"),
                        wxFD_SAVE | wxFD_PREVIEW | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxXmlDocument xmlDoc;

    wxXmlNode* root = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, L"i18n-check-settings");
    xmlDoc.SetRoot(root);

    wxXmlNode* node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"path");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_filePath));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"excluded-paths");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_excludedPaths));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"checks");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, std::to_wstring(m_options)));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"fuzzy-translations");
    node->AddChild(
        new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_fuzzyTranslations ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"log-messages-can-be-translated");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 m_logMessagesCanBeTranslated ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"allow-translating-punctuation-only-strings");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 m_allowTranslatingPunctuationOnlyStrings ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"exceptions-should-be-translatable");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 m_exceptionsShouldBeTranslatable ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"min-words-for-classifying-unavailable-string");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 std::to_wstring(m_minWordsForClassifyingUnavailableString)));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"min-cpp-version");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, std::to_wstring(m_minCppVersion)));

    if (!xmlDoc.Save(dialog.GetPath()))
        {
        wxMessageBox(_(L"Error saving project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        }

    m_projectDirty = false;
    }

//------------------------------------------------------
void I18NFrame::SaveProjectIfNeeded()
    {
    if (m_projectDirty)
        {
        if (wxMessageBox(_(L"Do you wish to save changes?"), _(L"Save Project"),
                         wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxID_YES)
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
    if (!m_activeSourceFile.empty() && m_editor->IsModified())
        {
        if (m_promptForFileSave)
            {
            wxRichMessageDialog msg(this,
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
                m_editor->SaveFile(m_activeSourceFile);
                }
            }
        else
            {
            m_editor->SaveFile(m_activeSourceFile);
            }
        }
    }

//------------------------------------------------------
void I18NFrame::Process()
    {
    SaveSourceFileIfNeeded();

    m_activeSourceFile.clear();
    m_editor->SetText(wxString{});

    std::wstring inputFolder{ m_filePath.c_str() };

    std::vector<std::wstring> excludedPaths;
    wxStringTokenizer tokenizer(
        m_excludedPaths, L";",
        wxStringTokenizerMode(wxTOKEN_STRTOK | wxTOKEN_RET_EMPTY | wxTOKEN_RET_EMPTY_ALL));
    while (tokenizer.HasMoreTokens())
        {
        std::wstring currentFolder{ tokenizer.GetNextToken().wc_string() };
        string_util::trim(currentFolder);
        excludedPaths.push_back(std::move(currentFolder));
        }
    // paths being ignored
    const auto excludedInfo = i18n_check::get_paths_files_to_exclude(inputFolder, excludedPaths);

    // input folder
    const auto filesToAnalyze = i18n_check::get_files_to_analyze(
        inputFolder, excludedInfo.excludedPaths, excludedInfo.excludedFiles);

    i18n_check::cpp_i18n_review cpp;
    cpp.set_style(static_cast<i18n_check::review_style>(m_options));
    cpp.log_messages_can_be_translatable(m_logMessagesCanBeTranslated);
    cpp.allow_translating_punctuation_only_strings(m_allowTranslatingPunctuationOnlyStrings);
    cpp.exceptions_should_be_translatable(m_exceptionsShouldBeTranslatable);
    cpp.set_min_words_for_classifying_unavailable_string(m_minWordsForClassifyingUnavailableString);
    cpp.set_min_cpp_version(m_minCppVersion);
    i18n_check::rc_file_review rc;
    rc.set_style(static_cast<i18n_check::review_style>(m_options));
    rc.allow_translating_punctuation_only_strings(m_allowTranslatingPunctuationOnlyStrings);
    i18n_check::po_file_review po;
    po.set_style(static_cast<i18n_check::review_style>(m_options));
    po.review_fuzzy_translations(m_fuzzyTranslations);

    std::vector<std::wstring> filesThatShouldBeConvertedToUTF8;
    std::vector<std::wstring> filesThatContainUTF8Signature;

    wxProgressDialog progressDlg(
        _(L"Analyzing Files"), _(L"Reviewing files for l10n/i18n issues..."), filesToAnalyze.size(),
        this,
        wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME |
            wxPD_REMAINING_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
    progressDlg.Centre();

    analyze(
        filesToAnalyze, cpp, rc, po, filesThatShouldBeConvertedToUTF8,
        filesThatContainUTF8Signature,
        [&progressDlg](const size_t currentFileIndex, const size_t fileCount,
                       const std::wstring& file)
        {
            progressDlg.SetTitle(wxString::Format(
                _(L"Processing %s of %s files..."),
                wxNumberFormatter::ToString(currentFileIndex, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxNumberFormatter::ToString(fileCount, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)));
            if (!progressDlg.Update(
                    currentFileIndex,
                    wxString::Format(_(L"Reviewing %s..."), wxFileName{ file }.GetFullName())))
                {
                return false;
                }
            return true;
        });

    std::wstringstream report = format_results(cpp, rc, po, filesThatShouldBeConvertedToUTF8,
                                               filesThatContainUTF8Signature, false);

    std::wstring fileName, lineNo, columnNo, warningValue, explanation, warningId;
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
            const wxString fileName = tokenizer.GetNextToken();
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

    m_projectBar->EnableButton(wxID_SAVE, true);
    m_projectBar->EnableButton(wxID_EDIT, true);
    m_editBar->EnableButton(XRCID("ID_EXPAND_ALL"), false);
    m_editBar->EnableButton(XRCID("ID_COLLAPSE_ALL"), false);
    }

//------------------------------------------------------
bool I18NApp::OnInit()
    {
    if (!wxApp::OnInit())
        {
        return false;
        }

    SetAppName(L"i18n-check");

    wxArtProvider::Push(new I18NArtProvider);

    // create the main application window
    I18NFrame* frame = new I18NFrame(GetAppName());
    frame->InitControls();
    frame->SetSize(frame->FromDIP(wxSize{ 1200, 1200 }));
    frame->CenterOnScreen();
    frame->Show(true);

    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, wxSize{ 32, 32 }));
    frame->SetIcon(appIcon);

    return true;
    }
