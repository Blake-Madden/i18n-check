///////////////////////////////////////////////////////////////////////////////
// Name:        projectdlg.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "projectdlg.h"

//-------------------------------------------------------------
NewProjectDialog::NewProjectDialog(
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxString& caption /*= _(L"New Project")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    CreateControls();
    Centre();

    // bind events
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnFolderButtonClick, this,
         NewProjectDialog::ID_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnExcludedFolderButtonClick, this,
         NewProjectDialog::ID_EXCLUDED_FOLDERS_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnExcludedFileButtonClick, this,
         NewProjectDialog::ID_EXCLUDED_FILES_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnOK, this, wxID_OK);
    }

//-------------------------------------------------------------
void NewProjectDialog::SetOptions(const i18n_check::review_style style)
    {
    m_options = static_cast<int>(style);

    m_notL10NAvailable = (m_options & i18n_check::review_style::check_not_available_for_l10n);
    m_suspectL10NString = (m_options & i18n_check::review_style::check_l10n_strings);
    m_suspectL10NUsage = (m_options & i18n_check::review_style::check_suspect_l10n_string_usage);
    m_printfMismatch = (m_options & i18n_check::review_style::check_mismatching_printf_commands);
    m_urlInL10NString = (m_options & i18n_check::review_style::check_mismatching_printf_commands);
    m_deprecatedMacro = (m_options & i18n_check::review_style::check_deprecated_macros);
    m_nonUTF8File = (m_options & i18n_check::review_style::check_utf8_encoded);
    m_UTF8FileWithBOM = (m_options & i18n_check::review_style::check_utf8_with_signature);
    m_unencodedExtASCII = (m_options & i18n_check::review_style::check_unencoded_ext_ascii);
    m_printfSingleNumber = (m_options & i18n_check::review_style::check_printf_single_number);
    m_numberAssignedToId = (m_options & i18n_check::review_style::check_number_assigned_to_id);
    m_dupValAssignedToIds =
        (m_options & i18n_check::review_style::check_duplicate_value_assigned_to_ids);
    m_malformedString = (m_options & i18n_check::review_style::check_malformed_strings);
    m_trailingSpaces = (m_options & i18n_check::review_style::check_trailing_spaces);
    m_fontIssue = (m_options & i18n_check::review_style::check_fonts);
    m_tabs = (m_options & i18n_check::review_style::check_tabs);
    m_wideLine = (m_options & i18n_check::review_style::check_line_width);
    m_commentMissingSpace = (m_options & i18n_check::review_style::check_line_width);

    TransferDataToWindow();
    }

//-------------------------------------------------------------
void NewProjectDialog::OnOK([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();

    if (m_filePath.empty() || !wxFileName::DirExists(m_filePath))
        {
        wxMessageBox(_(L"Please select a valid folder."), _(L"Invalid Folder"),
                     wxICON_EXCLAMATION | wxOK, this);
        return;
        }

    m_options = i18n_check::review_style::no_checks;

    if (m_notL10NAvailable)
        {
        m_options |= i18n_check::review_style::check_not_available_for_l10n;
        }
    if (m_suspectL10NString)
        {
        m_options |= i18n_check::review_style::check_l10n_strings;
        }
    if (m_suspectL10NUsage)
        {
        m_options |= i18n_check::review_style::check_suspect_l10n_string_usage;
        }
    if (m_printfMismatch)
        {
        m_options |= i18n_check::review_style::check_mismatching_printf_commands;
        }
    if (m_urlInL10NString)
        {
        m_options |= i18n_check::review_style::check_mismatching_printf_commands;
        }
    if (m_deprecatedMacro)
        {
        m_options |= i18n_check::review_style::check_deprecated_macros;
        }
    if (m_nonUTF8File)
        {
        m_options |= i18n_check::review_style::check_utf8_encoded;
        }
    if (m_UTF8FileWithBOM)
        {
        m_options |= i18n_check::review_style::check_utf8_with_signature;
        }
    if (m_unencodedExtASCII)
        {
        m_options |= i18n_check::review_style::check_unencoded_ext_ascii;
        }
    if (m_printfSingleNumber)
        {
        m_options |= i18n_check::review_style::check_printf_single_number;
        }
    if (m_numberAssignedToId)
        {
        m_options |= i18n_check::review_style::check_number_assigned_to_id;
        }
    if (m_dupValAssignedToIds)
        {
        m_options |= i18n_check::review_style::check_duplicate_value_assigned_to_ids;
        }
    if (m_malformedString)
        {
        m_options |= i18n_check::review_style::check_malformed_strings;
        }
    if (m_trailingSpaces)
        {
        m_options |= i18n_check::review_style::check_trailing_spaces;
        }
    if (m_fontIssue)
        {
        m_options |= i18n_check::review_style::check_fonts;
        }
    if (m_tabs)
        {
        m_options |= i18n_check::review_style::check_tabs;
        }
    if (m_wideLine)
        {
        m_options |= i18n_check::review_style::check_line_width;
        }
    if (m_commentMissingSpace)
        {
        m_options |= i18n_check::review_style::check_line_width;
        }

    switch (m_minCppVersion)
        {
    case 0:
        m_minCppVersion = 11;
        break;
    case 1:
        m_minCppVersion = 14;
        break;
    case 2:
        m_minCppVersion = 17;
        break;
    case 3:
        m_minCppVersion = 20;
        break;
    case 4:
        m_minCppVersion = 23;
        break;
        };

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }

//-------------------------------------------------------------
void NewProjectDialog::OnFolderButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    wxDirDialog dirDlg(this, _(L"Select Folder to Analyze"));
    dirDlg.SetPath(m_filePath);
    if (dirDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    m_filePath = dirDlg.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::OnExcludedFolderButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    wxDirDialog dirDlg(this, _(L"Select Subfolders to Ignore"));
    if (dirDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    if (m_excludedPaths.empty())
        {
        m_excludedPaths = dirDlg.GetPath();
        }
    else
        {
        m_excludedPaths += L"; " + dirDlg.GetPath();
        }
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::OnExcludedFileButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    wxFileDialog dialog(this, _(L"Select Files to Ignore"), wxString{}, wxString{},
                        _(L"All Files (*.*)|*.*"),
                        wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxArrayString paths;
    dialog.GetPaths(paths);
    wxString allPaths;
    for (const auto& path : paths)
        {
        allPaths += L"; " + path;
        }
    if (m_excludedPaths.empty())
        {
        if (allPaths.length() > 2)
            {
            allPaths.erase(0, 2);
            }
        m_excludedPaths = allPaths;
        }
    else
        {
        m_excludedPaths += allPaths;
        }
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::CreateControls()
    {
    wxTreebook* treeBook = new wxTreebook(this, wxID_ANY);

    wxBoxSizer* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

        {
        wxPanel* generalSettingsPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

            {
            wxStaticBoxSizer* fileBrowseBoxSizer =
                new wxStaticBoxSizer(wxHORIZONTAL, generalSettingsPage, _(L"Folder to analyze"));

            mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags().Expand().Border());

            wxTextCtrl* filePathEdit = new wxTextCtrl(
                fileBrowseBoxSizer->GetStaticBox(), wxID_ANY, wxString{}, wxDefaultPosition,
                wxSize(FromDIP(500), -1), wxTE_RICH2 | wxBORDER_THEME | wxTE_BESTWRAP,
                wxGenericValidator(&m_filePath));
            filePathEdit->AutoCompleteFileNames();
            filePathEdit->AutoCompleteDirectories();
            fileBrowseBoxSizer->Add(filePathEdit, wxSizerFlags{ 1 }.Expand());

            wxBitmapButton* fileBrowseButton =
                new wxBitmapButton(fileBrowseBoxSizer->GetStaticBox(), ID_FOLDER_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_BUTTON));
            fileBrowseBoxSizer->Add(fileBrowseButton, wxSizerFlags{}.CenterVertical());

            filePathEdit->SetFocus();
            }

            {
            wxStaticBoxSizer* fileBrowseBoxSizer =
                new wxStaticBoxSizer(wxHORIZONTAL, generalSettingsPage, _(L"Subfolders/files to ignore"));

            mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags{ 1 }.Expand().Border());

            wxTextCtrl* filePathEdit =
                new wxTextCtrl(fileBrowseBoxSizer->GetStaticBox(), wxID_ANY, wxString{},
                               wxDefaultPosition, wxSize(FromDIP(500), FromDIP(60)),
                               wxTE_RICH2 | wxTE_MULTILINE | wxBORDER_THEME | wxTE_BESTWRAP,
                               wxGenericValidator(&m_excludedPaths));
            filePathEdit->AutoCompleteFileNames();
            filePathEdit->AutoCompleteDirectories();
            fileBrowseBoxSizer->Add(filePathEdit, wxSizerFlags{ 1 }.Expand());

            wxBitmapButton* folderBrowseButton = new wxBitmapButton(
                fileBrowseBoxSizer->GetStaticBox(), ID_EXCLUDED_FOLDERS_BROWSE_BUTTON,
                wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_BUTTON));
            fileBrowseBoxSizer->Add(folderBrowseButton, wxSizerFlags{}.CenterVertical());
            
            wxBitmapButton* fileBrowseButton = new wxBitmapButton(
                fileBrowseBoxSizer->GetStaticBox(), ID_EXCLUDED_FILES_BROWSE_BUTTON,
                wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
            fileBrowseBoxSizer->Add(fileBrowseButton, wxSizerFlags{}.CenterVertical());
            }

        mainSizer->Add(
            new wxCheckBox(generalSettingsPage, wxID_ANY,
                           _(L"Check for strings not exposed for translation [notL10NAvailable]"),
                           wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_notL10NAvailable)),
            wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(
            new wxCheckBox(
                generalSettingsPage, wxID_ANY,
                _(L"Check for translatable strings that shouldn't be [suspectL10NString]"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_suspectL10NString)),
            wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Check for translatable strings being used in internal "
                                        L"contexts [suspectL10NUsage]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_suspectL10NUsage)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Check for translatable strings that contain URLs or "
                                        L"email addresses [urlInL10NString]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_urlInL10NString)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Check for deprecated text macros [deprecatedMacro]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_deprecatedMacro)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Check for printf()-like functions being used to just "
                                        L"format a number [printfSingleNumber]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_printfSingleNumber)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Check for malformed syntax in strings [malformedString]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_malformedString)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Strings passed to logging functions can be translatable"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_logMessagesCanBeTranslated)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(
            new wxCheckBox(generalSettingsPage, wxID_ANY,
                           _(L"Punctuation only strings can be translatable"), wxDefaultPosition,
                           wxDefaultSize, 0,
                           wxGenericValidator(&m_allowTranslatingPunctuationOnlyStrings)),
            wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Exception messages should be available for translation"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_exceptionsShouldBeTranslatable)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));

        wxBoxSizer* minWordSizer = new wxBoxSizer(wxHORIZONTAL);

        minWordSizer->Add(new wxStaticText(generalSettingsPage, wxID_STATIC,
                                           _(L"Minimum number of words that a string must have to "
                                             L"be considered translatable:"),
                                           wxDefaultPosition, wxDefaultSize),
                          0, wxALIGN_CENTER_VERTICAL);

        wxSpinCtrl* minWordCtrl =
            new wxSpinCtrl(generalSettingsPage, wxID_ANY,
                           std::to_wstring(m_minWordsForClassifyingUnavailableString),
                           wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1024);
        minWordCtrl->SetValidator(wxGenericValidator(&m_minWordsForClassifyingUnavailableString));
        minWordSizer->Add(minWordCtrl, wxSizerFlags().Border(wxLEFT));

        mainSizer->Add(minWordSizer, wxSizerFlags().Expand().Border());

        generalSettingsPage->SetSizer(mainSizer);
        treeBook->AddPage(generalSettingsPage, _(L"General"), true);
        }

        // C++
        {
        wxPanel* cppPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                      _(L"Check that files containing extended ASCII characters "
                                        L"are UTF-8 encoded [nonUTF8File]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_nonUTF8File)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                      _(L"Check for UTF-8 encoded files which start with a "
                                        L"BOM/UTF-8 signature [UTF8FileWithBOM]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_UTF8FileWithBOM)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                      _(L"Check for strings containing extended ASCII characters "
                                        L"that are not encoded [unencodedExtASCII]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_unencodedExtASCII)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                      _(L"Check for ID variables being assigned a hard-coded "
                                        L"number [numberAssignedToId]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_numberAssignedToId)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                      _(L"Check for the same value being assigned to different "
                                        L"ID variables [dupValAssignedToIds]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_dupValAssignedToIds)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));

        wxBoxSizer* cppVersionSizer = new wxBoxSizer(wxHORIZONTAL);

        cppVersionSizer->Add(
            new wxStaticText(
                cppPage, wxID_STATIC,
                _(L"C++ standard that should be assumed when issuing deprecated macro warnings:"),
                wxDefaultPosition, wxDefaultSize),
            0, wxALIGN_CENTER_VERTICAL);

        wxArrayString cppVersions;
        cppVersions.Add(L"11");
        cppVersions.Add(L"14");
        cppVersions.Add(L"17");
        cppVersions.Add(L"20");
        cppVersions.Add(L"23");
        wxChoice* cppVersionRadioBox =
            new wxChoice(cppPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, cppVersions, 0,
                         wxGenericValidator(&m_minCppVersion));
        cppVersionSizer->Add(cppVersionRadioBox, wxSizerFlags().Border(wxLEFT).Align(wxALIGN_LEFT));

        mainSizer->Add(cppVersionSizer, wxSizerFlags().Expand().Border());

        cppPage->SetSizer(mainSizer);
        treeBook->AddPage(cppPage, L"C++", false);
        }

        // translation catalogs
        {
        wxPanel* transPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticBoxSizer* poOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, transPage, _(L"PO Catalog Files"));

        poOptionsSizer->Add(
            new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                           _(L"Check for mismatching printf() commands between source "
                             L"and translation strings [printfMismatch]"),
                           wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_printfMismatch)),
            wxSizerFlags().Border().Align(wxALIGN_LEFT));

        poOptionsSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                           _(L"Review fuzzy translations"), wxDefaultPosition,
                                           wxDefaultSize, 0,
                                           wxGenericValidator(&m_fuzzyTranslations)),
                            wxSizerFlags().Border().Align(wxALIGN_LEFT));

        mainSizer->Add(poOptionsSizer, wxSizerFlags().Expand().Border());

        transPage->SetSizer(mainSizer);
        treeBook->AddPage(transPage, _(L"Translation Catalogs"), false);
        }

        // resource files
        {
        wxPanel* resourceFilesPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticBoxSizer* rcOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, resourceFilesPage, _(L"Windows RC Files"));

        rcOptionsSizer->Add(new wxCheckBox(rcOptionsSizer->GetStaticBox(), wxID_ANY,
                                           _(L"Check for font issues [fontIssue]"),
                                           wxDefaultPosition, wxDefaultSize, 0,
                                           wxGenericValidator(&m_fontIssue)),
                            wxSizerFlags().Border().Align(wxALIGN_LEFT));

        mainSizer->Add(rcOptionsSizer, wxSizerFlags().Expand().Border());

        resourceFilesPage->SetSizer(mainSizer);
        treeBook->AddPage(resourceFilesPage, _(L"Resource Files"), false);
        }

        // code formatting
        {
        wxPanel* codeFormattingPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(
            new wxCheckBox(codeFormattingPage, wxID_ANY,
                           _(L"Check for trailing spaces at the end of each line [trailingSpaces]"),
                           wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_trailingSpaces)),
            wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(codeFormattingPage, wxID_ANY, _(L"Check for tabs [tabs]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_tabs)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(new wxCheckBox(codeFormattingPage, wxID_ANY,
                                      _(L"Check for overly long lines [wideLine]"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_wideLine)),
                       wxSizerFlags().Border().Align(wxALIGN_LEFT));
        mainSizer->Add(
            new wxCheckBox(
                codeFormattingPage, wxID_ANY,
                _(L"Check that there is a space at the start of comments [commentMissingSpace]"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_commentMissingSpace)),
            wxSizerFlags().Border().Align(wxALIGN_LEFT));

        codeFormattingPage->SetSizer(mainSizer);
        treeBook->AddPage(codeFormattingPage, _(L"Code Formatting"), false);
        }

    mainDlgSizer->Add(treeBook, wxSizerFlags(1).Expand().Border());
    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL),
                      wxSizerFlags().Expand().Border());

    SetSizerAndFit(mainDlgSizer);
    }
