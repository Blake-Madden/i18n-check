//////////////////////////////////////////////////////////////////////////////
// Name:        i18napp.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "i18napp.h"

wxIMPLEMENT_APP(I18NApp);

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

        wxRibbonButtonBar* toolbar = new wxRibbonButtonBar(projectPanel);
        toolbar->AddButton(wxID_NEW, _(L"New"),
                           wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize{ 32, 32 }));
        toolbar->AddButton(wxID_EDIT, _(L"Edit"),
                           wxArtProvider::GetBitmap(wxART_EDIT, wxART_OTHER, wxSize{ 32, 32 }));

        toolbar->AddButton(XRCID("ID_EXPAND_ALL"), _(L"Expand All"),
                           wxArtProvider::GetBitmap(wxART_MINUS, wxART_OTHER, wxSize{ 32, 32 }));
        toolbar->AddButton(XRCID("ID_COLLAPSE_ALL"), _(L"Collapse All"),
                           wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize{ 32, 32 }));
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

    m_messageWindow = new wxHtmlWindow(splitter, wxID_ANY, wxDefaultPosition,
                                       FromDIP(wxSize{ 100, 150 }), wxHW_SCROLLBAR_AUTO);

    splitter->SplitHorizontally(m_resultsDataView, m_messageWindow, FromDIP(-150));
    mainSizer->Add(splitter, wxSizerFlags(1).Expand());

    SetSizer(mainSizer);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnNew, this, wxID_NEW);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnEdit, this, wxID_EDIT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnExpandAll, this, XRCID("ID_EXPAND_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &I18NFrame::OnCollapseAll, this, XRCID("ID_COLLAPSE_ALL"));
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
                 if (node->m_fileName == node->m_warningId)
                     {
                     m_messageWindow->SetPage(wxString{});
                     return;
                     }

                 wxString currentMessage = node->m_fileName + L"<br />";
                 if (node->m_line != -1 && node->m_column != -1)
                     {
                     currentMessage += wxString::Format(
                         _(L"Line: %s, Column: %s<br />"),
                         wxNumberFormatter::ToString(
                             node->m_line, 0,
                             wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                 wxNumberFormatter::Style::Style_WithThousandsSep),
                         wxNumberFormatter::ToString(
                             node->m_column, 0,
                             wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                 wxNumberFormatter::Style::Style_WithThousandsSep));
                     }

                 currentMessage += L"<br /><span style='font-weight: bold'>" + node->m_warningId +
                                   L"</span>: " + node->m_explaination;

                 if (!node->m_issue.empty() && node->m_warningId != L"[deprecatedMacro]" &&
                     node->m_warningId != L"[trailingSpaces]" &&
                     node->m_warningId != L"[unencodedExtASCII]" &&
                     node->m_warningId != L"[UTF8FileWithBOM]" &&
                     node->m_warningId != L"[nonUTF8File]")
                     {
                     currentMessage +=
                         L"<br /><br />Value:<br />&nbsp;&nbsp;&nbsp;&nbsp;" + node->m_issue;
                     }

                 m_messageWindow->SetPage(currentMessage);
                 }
         });
    }

//------------------------------------------------------
void I18NFrame::OnExpandAll([[maybe_unused]] wxCommandEvent&) { ExpandAll(); }

//------------------------------------------------------
void I18NFrame::OnCollapseAll([[maybe_unused]] wxCommandEvent&) { Collapse(); }

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
        }
    }

//------------------------------------------------------
void I18NFrame::OnNew([[maybe_unused]] wxCommandEvent&)
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
void I18NFrame::Process()
    {
    m_messageWindow->SetPage(wxString{});

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
    }

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
