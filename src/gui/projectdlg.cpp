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
    const bool showFileOptions /*= true*/, const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    : m_showFileOptions(showFileOptions)
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

    if (m_showFileOptions && (m_filePath.empty() || !wxFileName::DirExists(m_filePath)))
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

    m_minCppVersion = MinCppVersion();

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
    wxDirDialog dirDlg(this, _(L"Select Subfolders to Ignore"), wxString{},
                       wxDD_DEFAULT_STYLE | wxDD_MULTIPLE | wxDD_DIR_MUST_EXIST);
    if (dirDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    wxArrayString paths;
    dirDlg.GetPaths(paths);
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
    const auto buildCodeLabel = [](const wxString& label, wxWindow* parent)
    {
        wxStaticText* statLabel = new wxStaticText(parent, wxID_STATIC, L"[" + label + L"]",
                                                   wxDefaultPosition, wxDefaultSize);
        statLabel->SetFont(statLabel->GetFont().Bold());
        return statLabel;
    };

    wxTreebook* treeBook = new wxTreebook(this, wxID_ANY);

    wxBoxSizer* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

        {
        wxPanel* generalSettingsPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        if (m_showFileOptions)
            {
                {
                wxStaticBoxSizer* fileBrowseBoxSizer = new wxStaticBoxSizer(
                    wxHORIZONTAL, generalSettingsPage, _(L"Folder to analyze"));

                mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags{}.Expand().Border());

                wxTextCtrl* filePathEdit = new wxTextCtrl(
                    fileBrowseBoxSizer->GetStaticBox(), wxID_ANY, wxString{}, wxDefaultPosition,
                    wxSize(FromDIP(500), -1), wxTE_RICH2 | wxBORDER_THEME | wxTE_BESTWRAP,
                    wxGenericValidator(&m_filePath));
                filePathEdit->AutoCompleteFileNames();
                filePathEdit->AutoCompleteDirectories();
                fileBrowseBoxSizer->Add(filePathEdit, wxSizerFlags{ 1 }.Expand());

                wxBitmapButton* fileBrowseButton = new wxBitmapButton(
                    fileBrowseBoxSizer->GetStaticBox(), ID_FOLDER_BROWSE_BUTTON,
                    wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_BUTTON));
                fileBrowseBoxSizer->Add(fileBrowseButton, wxSizerFlags{}.CenterVertical());

                filePathEdit->SetFocus();
                }

                // files/folders to ignore
                {
                wxStaticBoxSizer* fileBrowseBoxSizer = new wxStaticBoxSizer(
                    wxHORIZONTAL, generalSettingsPage, _(L"Subfolders/files to ignore"));

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
            }

        wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
        size_t currentRow{ 0 };

        gbSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                    _(L"Check for strings not exposed for translation"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_notL10NAvailable)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"notL10NAvailable"), generalSettingsPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                    _(L"Check for translatable strings that shouldn't be"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_suspectL10NString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"suspectL10NString"), generalSettingsPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                    _(L"Check for translatable strings being used "
                                      "in internal contexts"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_suspectL10NUsage)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"suspectL10NUsage"), generalSettingsPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                    _(L"Check for translatable strings that contain URLs or "
                                      "email addresses"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_urlInL10NString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"urlInL10NString"), generalSettingsPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                    _(L"Check for deprecated macros and functions"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_deprecatedMacro)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"deprecatedMacro"), generalSettingsPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                    _(L"Check for printf()-like functions being used to just "
                                      "format a number"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_printfSingleNumber)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"printfSingleNumber"), generalSettingsPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                    _(L"Check for malformed syntax in strings"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_malformedString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"malformedString"), generalSettingsPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        mainSizer->Add(gbSizer, wxSizerFlags{}.Border(wxLEFT));

        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Strings passed to logging functions can be translatable"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_logMessagesCanBeTranslated)),
                       wxSizerFlags{}.Border().Left());
        mainSizer->Add(
            new wxCheckBox(generalSettingsPage, wxID_ANY,
                           _(L"Punctuation only strings can be translatable"), wxDefaultPosition,
                           wxDefaultSize, 0,
                           wxGenericValidator(&m_allowTranslatingPunctuationOnlyStrings)),
            wxSizerFlags{}.Border().Left());
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Exception messages should be available for translation"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_exceptionsShouldBeTranslatable)),
                       wxSizerFlags{}.Border().Left());

        wxBoxSizer* minWordSizer = new wxBoxSizer(wxHORIZONTAL);

        minWordSizer->Add(new wxStaticText(generalSettingsPage, wxID_STATIC,
                                           _(L"Minimum number of words that a string must have to "
                                             "be considered translatable:"),
                                           wxDefaultPosition, wxDefaultSize),
                          wxSizerFlags{}.CenterVertical());

        wxSpinCtrl* minWordCtrl =
            new wxSpinCtrl(generalSettingsPage, wxID_ANY,
                           std::to_wstring(m_minWordsForClassifyingUnavailableString),
                           wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1024);
        minWordCtrl->SetValidator(wxGenericValidator(&m_minWordsForClassifyingUnavailableString));
        minWordSizer->Add(minWordCtrl, wxSizerFlags{}.Border(wxLEFT));

        mainSizer->Add(minWordSizer, wxSizerFlags{}.Expand().Border());

        generalSettingsPage->SetSizer(mainSizer);
        treeBook->AddPage(generalSettingsPage, _(L"General"), true);
        }

        // C++
        {
        wxPanel* cppPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
        size_t currentRow{ 0 };

        gbSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                    _(L"Check that files containing extended ASCII characters "
                                      "are UTF-8 encoded"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_nonUTF8File)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"nonUTF8File"), cppPage), wxGBPosition(currentRow++, 1),
                     wxGBSpan{});

        gbSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                    _(L"Check for UTF-8 encoded files which start with a "
                                      "BOM/UTF-8 signature"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_UTF8FileWithBOM)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"UTF8FileWithBOM"), cppPage), wxGBPosition(currentRow++, 1),
                     wxGBSpan{});

        gbSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                    _(L"Check for strings containing extended ASCII characters "
                                      "that are not encoded"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_unencodedExtASCII)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"unencodedExtASCII"), cppPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                    _(L"Check for ID variables being assigned a "
                                      "hard-coded number"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_numberAssignedToId)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"numberAssignedToId"), cppPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(cppPage, wxID_ANY,
                                    _(L"Check for the same value being assigned to different "
                                      "ID variables"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_dupValAssignedToIds)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"dupValAssignedToIds"), cppPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        wxBoxSizer* cppVersionSizer = new wxBoxSizer(wxHORIZONTAL);

        cppVersionSizer->Add(
            new wxStaticText(
                cppPage, wxID_STATIC,
                _(L"C++ standard that should be assumed when issuing deprecated macro warnings:"),
                wxDefaultPosition, wxDefaultSize),
            wxSizerFlags{}.CenterVertical());

        wxArrayString cppVersions;
        cppVersions.Add(L"11");
        cppVersions.Add(L"14");
        cppVersions.Add(L"17");
        cppVersions.Add(L"20");
        cppVersions.Add(L"23");
        wxChoice* cppVersionRadioBox =
            new wxChoice(cppPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, cppVersions, 0,
                         wxGenericValidator(&m_minCppVersion));
        cppVersionSizer->Add(cppVersionRadioBox, wxSizerFlags{}.Border(wxLEFT).Left());

        gbSizer->Add(cppVersionSizer, wxGBPosition(currentRow, 0), wxGBSpan{});

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(gbSizer);
        mainSizer->AddStretchSpacer();

        cppPage->SetSizer(mainSizer);
        treeBook->AddPage(cppPage, L"C++", false);
        }

        // translation catalogs
        {
        wxPanel* transPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxStaticBoxSizer* poOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, transPage, _(L"PO Catalog Files"));

        wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
        size_t currentRow{ 0 };

        gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Check for mismatching printf() commands"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_printfMismatch)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"printfMismatch"), poOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Review fuzzy translations"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_fuzzyTranslations)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        poOptionsSizer->Add(gbSizer);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(poOptionsSizer, wxSizerFlags{}.Expand().Border());

        transPage->SetSizer(mainSizer);
        treeBook->AddPage(transPage, _(L"Translation Catalogs"), false);
        }

        // resource files
        {
        wxPanel* resourceFilesPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxStaticBoxSizer* rcOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, resourceFilesPage, _(L"Windows RC Files"));

        wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
        size_t currentRow{ 0 };

        gbSizer->Add(new wxCheckBox(rcOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Check for font issues"), wxDefaultPosition, wxDefaultSize,
                                    0, wxGenericValidator(&m_fontIssue)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"fontIssue"), rcOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});
        rcOptionsSizer->Add(gbSizer);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(rcOptionsSizer, wxSizerFlags{}.Expand().Border());

        resourceFilesPage->SetSizer(mainSizer);
        treeBook->AddPage(resourceFilesPage, _(L"Resource Files"), false);
        }

        // code formatting
        {
        wxPanel* codeFormattingPage =
            new wxPanel(treeBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
        size_t currentRow{ 0 };

        gbSizer->Add(new wxCheckBox(codeFormattingPage, wxID_ANY,
                                    _(L"Check lines for trailing spaces"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_trailingSpaces)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"trailingSpaces"), codeFormattingPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(codeFormattingPage, wxID_ANY, _(L"Check for tabs"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_tabs)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"tabs"), codeFormattingPage), wxGBPosition(currentRow++, 1),
                     wxGBSpan{});

        gbSizer->Add(new wxCheckBox(codeFormattingPage, wxID_ANY, _(L"Check for overly long lines"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_wideLine)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"wideLine"), codeFormattingPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(codeFormattingPage, wxID_ANY,
                                    _(L"Check that comments start with a space"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_commentMissingSpace)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(_(L"commentMissingSpace"), codeFormattingPage),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(gbSizer);
        mainSizer->AddStretchSpacer();

        codeFormattingPage->SetSizer(mainSizer);
        treeBook->AddPage(codeFormattingPage, _(L"Code Formatting"), false);
        }

    mainDlgSizer->Add(treeBook, wxSizerFlags{}.Expand().Border());
    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL),
                      wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainDlgSizer);
    }
