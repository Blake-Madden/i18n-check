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
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
    const wxString& caption /*= _(L"Select Directory")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN|wxRESIZE_BORDER*/)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    CreateControls();
    Centre();

    // bind events
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnFolderButtonClick, this,
         NewProjectDialog::ID_FOLDER_BROWSE_BUTTON);
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
    wxDirDialog dirDlg(this);
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
void NewProjectDialog::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags().Expand().Border());

    wxTextCtrl* filePathEdit =
        new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition,
                       wxSize(FromDIP(wxSize(500, 100)).GetWidth(), -1),
                       wxTE_RICH2 | wxBORDER_THEME, wxGenericValidator(&m_filePath));
    filePathEdit->AutoCompleteFileNames();
    fileBrowseBoxSizer->Add(filePathEdit, 1, wxEXPAND);

    wxBitmapButton* fileBrowseButton =
        new wxBitmapButton(this, ID_FOLDER_BROWSE_BUTTON,
                           wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
    fileBrowseBoxSizer->Add(fileBrowseButton, 0, wxALIGN_CENTER_VERTICAL);

    wxStaticBoxSizer* allOptionsSizer =
        new wxStaticBoxSizer(wxVERTICAL, this, _(L"General Options"));

    allOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY,
                       _(L"Check for strings not exposed for translation ([notL10NAvailable])"),
                       wxDefaultPosition, wxDefaultSize, 0,
                       wxGenericValidator(&m_notL10NAvailable)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY,
                       _(L"Check for translatable strings that shouldn't be ([suspectL10NString])"),
                       wxDefaultPosition, wxDefaultSize, 0,
                       wxGenericValidator(&m_suspectL10NString)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                        _(L"Check for translatable strings being used in internal "
                                          L"contexts ([suspectL10NUsage])"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_suspectL10NUsage)),
                         wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                        _(L"Check for translatable strings that contain URLs or "
                                          L"email addresses ([urlInL10NString])"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_urlInL10NString)),
                         wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY, _(L"Check for deprecated text macros ([deprecatedMacro])"),
                       wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_deprecatedMacro)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                        _(L"Check for printf()-like functions being used to just "
                                          L"format a number ([printfSingleNumber])"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_printfSingleNumber)),
                         wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY,
                       _(L"Check for malformed syntax in strings ([malformedString])"),
                       wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_malformedString)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(
        new wxCheckBox(
            this, wxID_ANY, _(L"Strings passed to logging functions can be translatable"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_logMessagesCanBeTranslated)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY, _(L"Punctuation only strings can be translatable"),
                       wxDefaultPosition, wxDefaultSize, 0,
                       wxGenericValidator(&m_allowTranslatingPunctuationOnlyStrings)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    allOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY, _(L"Exception messages should be available for translation"),
                       wxDefaultPosition, wxDefaultSize, 0,
                       wxGenericValidator(&m_exceptionsShouldBeTranslatable)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));

    wxBoxSizer* minWordSizer = new wxBoxSizer(wxHORIZONTAL);

    minWordSizer->Add(
        new wxStaticText(
            this, wxID_STATIC,
            _(L"Minimum number of words that a string must have to be considered translatable:"),
            wxDefaultPosition, wxDefaultSize),
        0, wxALIGN_CENTER_VERTICAL);

    wxSpinCtrl* minWordCtrl =
        new wxSpinCtrl(this, wxID_ANY, std::to_wstring(m_minWordsForClassifyingUnavailableString),
                       wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1024);
    minWordCtrl->SetValidator(wxGenericValidator(&m_minWordsForClassifyingUnavailableString));
    minWordSizer->Add(minWordCtrl, wxSizerFlags().Border(wxLEFT));

    allOptionsSizer->Add(minWordSizer, wxSizerFlags().Expand().Border());

    mainSizer->Add(allOptionsSizer, wxSizerFlags().Expand().Border());

    wxStaticBoxSizer* cppOptionsSizer =
        new wxStaticBoxSizer(wxVERTICAL, this, _(L"C++ Specific Options"));

    cppOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                        _(L"Check that files containing extended ASCII characters "
                                          L"are UTF-8 encoded ([nonUTF8File])"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_nonUTF8File)),
                         wxSizerFlags().Border().Align(wxALIGN_LEFT));
    cppOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                        _(L"Check for UTF-8 encoded files which start with a "
                                          L"BOM/UTF-8 signature ([UTF8FileWithBOM])"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_UTF8FileWithBOM)),
                         wxSizerFlags().Border().Align(wxALIGN_LEFT));
    cppOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                        _(L"Check for strings containing extended ASCII characters "
                                          L"that are not encoded ([unencodedExtASCII])"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_unencodedExtASCII)),
                         wxSizerFlags().Border().Align(wxALIGN_LEFT));
    cppOptionsSizer->Add(
        new wxCheckBox(
            this, wxID_ANY,
            _(L"Check for ID variables being assigned a hard-coded number ([numberAssignedToId])"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_numberAssignedToId)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    cppOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                        _(L"Check for the same value being assigned to different "
                                          L"ID variables ([dupValAssignedToIds])"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_dupValAssignedToIds)),
                         wxSizerFlags().Border().Align(wxALIGN_LEFT));

    wxBoxSizer* cppVersionSizer = new wxBoxSizer(wxHORIZONTAL);

    cppVersionSizer->Add(
        new wxStaticText(
            this, wxID_STATIC,
            _(L"C++ standard that should be assumed when issuing deprecated macro warnings:"),
            wxDefaultPosition, wxDefaultSize),
        0, wxALIGN_CENTER_VERTICAL);

    wxArrayString cppVersions;
    cppVersions.Add(_(L"11"));
    cppVersions.Add(_(L"14"));
    cppVersions.Add(_(L"17"));
    cppVersions.Add(_(L"20"));
    cppVersions.Add(_(L"23"));
    wxChoice* cppVersionRadioBox =
        new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, cppVersions, 0,
                     wxGenericValidator(&m_minCppVersion));
    cppVersionSizer->Add(cppVersionRadioBox, wxSizerFlags().Border(wxLEFT).Align(wxALIGN_LEFT));

    cppOptionsSizer->Add(cppVersionSizer, wxSizerFlags().Expand().Border());

    mainSizer->Add(cppOptionsSizer, wxSizerFlags().Expand().Border());

    wxStaticBoxSizer* poOptionsSizer =
        new wxStaticBoxSizer(wxVERTICAL, this, _(L"PO Catalog Files"));

    poOptionsSizer->Add(new wxCheckBox(this, wxID_ANY,
                                       _(L"Check for mismatching printf() commands between source "
                                         L"and translation strings ([printfMismatch])"),
                                       wxDefaultPosition, wxDefaultSize, 0,
                                       wxGenericValidator(&m_printfMismatch)),
                        wxSizerFlags().Border().Align(wxALIGN_LEFT));

    poOptionsSizer->Add(new wxCheckBox(this, wxID_ANY, _(L"Review fuzzy translations"),
                                       wxDefaultPosition, wxDefaultSize, 0,
                                       wxGenericValidator(&m_fuzzyTranslations)),
                        wxSizerFlags().Border().Align(wxALIGN_LEFT));

    mainSizer->Add(poOptionsSizer, wxSizerFlags().Expand().Border());

    wxStaticBoxSizer* rcOptionsSizer =
        new wxStaticBoxSizer(wxVERTICAL, this, _(L"Windows RC Files"));

    rcOptionsSizer->Add(new wxCheckBox(this, wxID_ANY, _(L"Check for font issues ([fontIssue])"),
                                       wxDefaultPosition, wxDefaultSize, 0,
                                       wxGenericValidator(&m_fontIssue)),
                        wxSizerFlags().Border().Align(wxALIGN_LEFT));

    mainSizer->Add(rcOptionsSizer, wxSizerFlags().Expand().Border());

    wxStaticBoxSizer* formattingOptionsSizer =
        new wxStaticBoxSizer(wxVERTICAL, this, _(L"Source File Formatting Options"));

    formattingOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY,
                       _(L"Check for trailing spaces at the end of each line ([trailingSpaces])"),
                       wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_trailingSpaces)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    formattingOptionsSizer->Add(new wxCheckBox(this, wxID_ANY, _(L"Check for tabs ([tabs])"),
                                               wxDefaultPosition, wxDefaultSize, 0,
                                               wxGenericValidator(&m_tabs)),
                                wxSizerFlags().Border().Align(wxALIGN_LEFT));
    formattingOptionsSizer->Add(
        new wxCheckBox(this, wxID_ANY, _(L"Check for overly long lines ([wideLine])"),
                       wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_wideLine)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));
    formattingOptionsSizer->Add(
        new wxCheckBox(
            this, wxID_ANY,
            _(L"Check that there is a space at the start of comments ([commentMissingSpace])"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_commentMissingSpace)),
        wxSizerFlags().Border().Align(wxALIGN_LEFT));

    mainSizer->Add(formattingOptionsSizer, wxSizerFlags().Expand().Border());

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().Border());

    SetSizerAndFit(mainSizer);

    filePathEdit->SetFocus();
    }
