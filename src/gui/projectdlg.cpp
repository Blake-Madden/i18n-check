/********************************************************************************
 * Copyright (c) 2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

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
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &NewProjectDialog::OnHelpClicked, this, wxID_HELP);
    Bind(wxEVT_HELP, &NewProjectDialog::OnContextHelp, this);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnFolderButtonClick, this,
         NewProjectDialog::ID_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnFileButtonClick, this,
         NewProjectDialog::ID_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnExcludedFolderButtonClick, this,
         NewProjectDialog::ID_EXCLUDED_FOLDERS_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnExcludedFileButtonClick, this,
         NewProjectDialog::ID_EXCLUDED_FILES_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnOK, this, wxID_OK);
    Bind(
        wxEVT_CHOICE,
        [this](wxCommandEvent& evt)
        {
            if (m_pseudoSurroundingBracketsCheckbox != nullptr)
                {
                m_pseudoSurroundingBracketsCheckbox->Enable(evt.GetSelection() != 0);
                }
            if (m_pseudoTrackCheckbox != nullptr)
                {
                m_pseudoTrackCheckbox->Enable(evt.GetSelection() != 0);
                }
            if (m_pseudoIncreaseSlider != nullptr)
                {
                m_pseudoIncreaseSlider->Enable(evt.GetSelection() != 0);
                }
            if (m_pseudoSliderLabel != nullptr)
                {
                m_pseudoSliderLabel->Enable(evt.GetSelection() != 0);
                }
            if (m_pseudoSliderPercentLabel != nullptr)
                {
                m_pseudoSliderPercentLabel->Enable(evt.GetSelection() != 0);
                }
        },
        ID_PSEUDO_METHODS);
    }

//-------------------------------------------------------------
void NewProjectDialog::SetOptions(const i18n_check::review_style style)
    {
    m_options = static_cast<int64_t>(style);

    m_notL10NAvailable = (m_options & i18n_check::review_style::check_not_available_for_l10n);
    m_suspectL10NString = (m_options & i18n_check::review_style::check_l10n_strings);
    m_suspectL10NUsage = (m_options & i18n_check::review_style::check_suspect_l10n_string_usage);
    m_suspectI18NUsage = (m_options & i18n_check::review_style::check_suspect_i18n_usage);
    m_printfMismatch = (m_options & i18n_check::review_style::check_mismatching_printf_commands);
    m_acceleratorMismatch = (m_options & i18n_check::review_style::check_accelerators);
    m_transConsistency = (m_options & i18n_check::review_style::check_consistency);
    m_needsContext = (m_options & i18n_check::review_style::check_needing_context);
    m_urlInL10NString = (m_options & i18n_check::review_style::check_l10n_contains_url);
    m_spacesAroundL10NString =
        (m_options & i18n_check::review_style::check_l10n_has_surrounding_spaces);
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
    m_commentMissingSpace = (m_options & i18n_check::review_style::check_space_after_comment);

    TransferDataToWindow();
    }

//-------------------------------------------------------------
void NewProjectDialog::OnOK([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();

    if (m_showFileOptions && (m_filePath.empty() || (!wxFileName::DirExists(m_filePath) &&
                                                     !wxFileName::FileExists(m_filePath))))
        {
        wxMessageBox(_(L"Please select a valid file folder."), _(L"Invalid Input"),
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
    if (m_suspectI18NUsage)
        {
        m_options |= i18n_check::review_style::check_suspect_i18n_usage;
        }
    if (m_printfMismatch)
        {
        m_options |= i18n_check::review_style::check_mismatching_printf_commands;
        }
    if (m_acceleratorMismatch)
        {
        m_options |= i18n_check::review_style::check_accelerators;
        }
    if (m_transConsistency)
        {
        m_options |= i18n_check::review_style::check_consistency;
        }
    if (m_needsContext)
        {
        m_options |= i18n_check::review_style::check_needing_context;
        }
    if (m_urlInL10NString)
        {
        m_options |= i18n_check::review_style::check_l10n_contains_url;
        }
    if (m_spacesAroundL10NString)
        {
        m_options |= i18n_check::review_style::check_l10n_has_surrounding_spaces;
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
        m_options |= i18n_check::review_style::check_space_after_comment;
        }

    m_minCppVersion = MinCppVersion();

    if (m_exclusionList != nullptr)
        {
        m_exclusionList->GetStrings(m_excludedPaths);
        }
    m_ignoredVarsList->GetStrings(m_varsToIgnore);

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
void NewProjectDialog::OnFileButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    wxFileDialog dialog(
        this, _(L"Select Files to Analyze"), wxString{}, wxString{},
        _(L"Source Files (*.cpp; *.c; *.h; *.hpp)|*.cpp;*.c;*.h;*.hpp|"
          "gettext Catalogs (*.po)|*.po|Windows Resource Files (*.rc)|*.rc|"
          "All Supported Files|*.cpp;*.c;*.h;*.hpp;*.po;*.rc"),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_filePath = dialog.GetPath();
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
    m_excludedPaths.insert(m_excludedPaths.end(), paths.begin(), paths.end());
    m_exclusionList->SetStrings(m_excludedPaths);

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
    m_excludedPaths.insert(m_excludedPaths.end(), paths.begin(), paths.end());
    m_exclusionList->SetStrings(m_excludedPaths);

    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::SetAllOptions(const I18NOptions& options)
    {
    SetOptions(static_cast<i18n_check::review_style>(options.m_options));
    m_filePath = options.m_filePath;
    m_excludedPaths = options.m_excludedPaths;
    m_varsToIgnore = options.m_varsToIgnore;
    m_fuzzyTranslations = options.m_fuzzyTranslations;
    m_widthPseudoIncrease = options.m_widthPseudoIncrease;
    m_addPseudoTransBrackets = options.m_addPseudoTransBrackets;
    m_pseudoTrack = options.m_pseudoTrack;
    m_pseudoTranslationMethod = static_cast<int>(options.m_pseudoTranslationMethod);
    m_logMessagesCanBeTranslated = options.m_logMessagesCanBeTranslated;
    m_allowTranslatingPunctuationOnlyStrings = options.m_allowTranslatingPunctuationOnlyStrings;
    m_exceptionsShouldBeTranslatable = options.m_exceptionsShouldBeTranslatable;
    m_verbose = options.m_verbose;
    m_minWordsForClassifyingUnavailableString = options.m_minWordsForClassifyingUnavailableString;
    MinCppVersion(options.m_minCppVersion);
    if (m_pseudoSurroundingBracketsCheckbox != nullptr)
        {
        m_pseudoSurroundingBracketsCheckbox->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoTrackCheckbox != nullptr)
        {
        m_pseudoTrackCheckbox->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoIncreaseSlider != nullptr)
        {
        m_pseudoIncreaseSlider->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoSliderLabel != nullptr)
        {
        m_pseudoSliderLabel->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoSliderPercentLabel != nullptr)
        {
        m_pseudoSliderPercentLabel->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_exclusionList != nullptr)
        {
        m_exclusionList->SetStrings(m_excludedPaths);
        }
    if (m_ignoredVarsList != nullptr)
        {
        m_ignoredVarsList->SetStrings(m_varsToIgnore);
        }
    TransferDataToWindow();
    }

//-------------------------------------------------------------
void NewProjectDialog::CreateControls()
    {
    const auto buildCodeLabel = [](const wxString& label, wxWindow* parent) -> wxWindow*
    {
        if (wxGetMouseState().ShiftDown())
            {
            wxStaticText* statLabel = new wxStaticText(parent, wxID_STATIC, L"[" + label + L"]");
            statLabel->SetFont(statLabel->GetFont().Bold());
            statLabel->SetForegroundColour(wxSystemSettings::GetAppearance().IsDark() ?
                                               wxColour{ L"#89CFF0" } :
                                               wxColour{ L"#0095B6" });
            return statLabel;
            }
        else
            {
            return new wxStaticText(parent, wxID_STATIC, wxString{}, wxDefaultPosition,
                                    wxDefaultSize);
            }
    };

    wxListbook* listBook = new wxListbook(this, wxID_ANY);
    const wxSize imageSize{ 32, 32 };
    wxBookCtrlBase::Images images;
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_OTHER, imageSize));
    images.push_back(wxArtProvider::GetBitmapBundle(L"ID_CODE", wxART_OTHER, imageSize));
    images.push_back(wxArtProvider::GetBitmapBundle(L"ID_TRANSLATIONS", wxART_OTHER, imageSize));
    images.push_back(wxArtProvider::GetBitmapBundle(L"ID_CHECK", wxART_OTHER, imageSize));
    listBook->SetImages(images);

    wxBoxSizer* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

    // input options
    if (m_showFileOptions)
        {
        wxPanel* inputPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

            {
            wxStaticBoxSizer* fileBrowseBoxSizer =
                new wxStaticBoxSizer(wxHORIZONTAL, inputPage, _(L"Folder/file to analyze"));

            mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags{}.Expand().Border());

            wxTextCtrl* filePathEdit = new wxTextCtrl(
                fileBrowseBoxSizer->GetStaticBox(), wxID_ANY, wxString{}, wxDefaultPosition,
                wxSize{ FromDIP(400), -1 }, wxTE_RICH2 | wxBORDER_THEME | wxTE_BESTWRAP,
                wxGenericValidator(&m_filePath));
            filePathEdit->AutoCompleteFileNames();
            filePathEdit->AutoCompleteDirectories();
            fileBrowseBoxSizer->Add(filePathEdit, wxSizerFlags{ 1 }.Expand());

            wxBitmapButton* folderBrowseButton =
                new wxBitmapButton(fileBrowseBoxSizer->GetStaticBox(), ID_FOLDER_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_BUTTON));
            fileBrowseBoxSizer->Add(folderBrowseButton, wxSizerFlags{}.CenterVertical());

            wxBitmapButton* fileBrowseButton =
                new wxBitmapButton(fileBrowseBoxSizer->GetStaticBox(), ID_FILE_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_NEW, wxART_BUTTON));
            fileBrowseBoxSizer->Add(fileBrowseButton, wxSizerFlags{}.CenterVertical());

            filePathEdit->SetFocus();
            }

            // files/folders to ignore
            {
            wxBoxSizer* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);

            mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags{ 1 }.Expand().Border());

            m_exclusionList =
                new wxEditableListBox(inputPage, wxID_ANY, _(L"Subfolders/files to ignore"));
            fileBrowseBoxSizer->Add(m_exclusionList, wxSizerFlags{ 1 }.Expand());

            wxBitmapButton* folderExcludeBrowseButton =
                new wxBitmapButton(inputPage, ID_EXCLUDED_FOLDERS_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_BUTTON));
            fileBrowseBoxSizer->Add(folderExcludeBrowseButton);

            wxBitmapButton* fileExcludeBrowseButton =
                new wxBitmapButton(inputPage, ID_EXCLUDED_FILES_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_NEW, wxART_BUTTON));
            fileBrowseBoxSizer->Add(fileExcludeBrowseButton);
            }

        inputPage->SetSizer(mainSizer);
        listBook->AddPage(inputPage, _(L"Input"), true, 0);
        }

        // source code options
        {
        wxPanel* generalSettingsPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticBoxSizer* checkOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, generalSettingsPage, _(L"Run the following checks:"));

        wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
        size_t currentRow{ 0 };

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Strings not exposed for translation"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_notL10NAvailable)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"notL10NAvailable", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        m_ignoredVarsList = new wxEditableListBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                                  _(L"Ignore strings assigned to variables named:"),
                                                  wxDefaultPosition, wxSize{ -1, FromDIP(100) });
        gbSizer->Add(m_ignoredVarsList, wxGBPosition(currentRow++, 0), wxGBSpan{ 1, 2 }, wxLEFT,
                     wxSizerFlags::GetDefaultBorder() * 3);

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Translatable strings that shouldn't be"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_suspectL10NString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"suspectL10NString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Translatable strings being used "
                                      "in debug functions"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_suspectL10NUsage)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"suspectL10NUsage", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Translatable strings that contain URLs or "
                                      "email addresses"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_urlInL10NString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"urlInL10NString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Translatable strings that are surrounded by spaces"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_spacesAroundL10NString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"spacesAroundL10NString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});
        
        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Suspect i18n function usage"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_suspectI18NUsage)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"suspectI18NUsage", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(
                         checkOptionsSizer->GetStaticBox(), wxID_ANY,
                         _(L"Check for ambiguous strings that lack a translator comment"),
                         wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_needsContext)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"L10NStringNeedsContext", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Deprecated macros and functions"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_deprecatedMacro)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"deprecatedMacro", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"printf functions being used to just "
                                      "format a number"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_printfSingleNumber)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"printfSingleNumber", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Malformed syntax in strings"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_malformedString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"malformedString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        checkOptionsSizer->Add(gbSizer, wxSizerFlags{}.Border());

        mainSizer->Add(checkOptionsSizer, wxSizerFlags{}.Expand().Border());

        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Strings passed to logging functions can be translatable"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_logMessagesCanBeTranslated)),
                       wxSizerFlags{}.Border().Left());
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Exception messages should be available for translation"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_exceptionsShouldBeTranslatable)),
                       wxSizerFlags{}.Border().Left());
        mainSizer->Add(
            new wxCheckBox(generalSettingsPage, wxID_ANY,
                           _(L"Punctuation only strings can be translatable"), wxDefaultPosition,
                           wxDefaultSize, 0,
                           wxGenericValidator(&m_allowTranslatingPunctuationOnlyStrings)),
            wxSizerFlags{}.Border().Left());

        wxBoxSizer* minWordSizer = new wxBoxSizer(wxHORIZONTAL);

        minWordSizer->Add(new wxStaticText(generalSettingsPage, wxID_STATIC,
                                           _(L"Minimum words for a string to "
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

        wxBoxSizer* cppVersionSizer = new wxBoxSizer(wxHORIZONTAL);

        cppVersionSizer->Add(new wxStaticText(generalSettingsPage, wxID_STATIC,
                                              _(L"C++ standard when issuing deprecation warnings:"),
                                              wxDefaultPosition, wxDefaultSize),
                             wxSizerFlags{}.CenterVertical());

        wxArrayString cppVersions;
        cppVersions.Add(L"2011");
        cppVersions.Add(L"2014");
        cppVersions.Add(L"2017");
        cppVersions.Add(L"2020");
        cppVersions.Add(L"2023");
        wxChoice* cppVersionRadioBox =
            new wxChoice(generalSettingsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                         cppVersions, 0, wxGenericValidator(&m_minCppVersion));
        cppVersionSizer->Add(cppVersionRadioBox, wxSizerFlags{}.Border(wxLEFT).Left());

        mainSizer->Add(cppVersionSizer, wxSizerFlags{}.Expand().Border());

        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY, _(L"Include verbose warnings"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_verbose)),
                       wxSizerFlags{}.Border().Left());

        generalSettingsPage->SetSizer(mainSizer);
        listBook->AddPage(generalSettingsPage, _(L"Source Code"), !m_showFileOptions, 1);
        }

        // resource files
        {
        wxPanel* transPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxStaticBoxSizer* poOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, transPage, _(L"Translation catalogs"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for inconsistent printf format specifiers"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_printfMismatch)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"printfMismatch", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for inconsistent keyboard accelerators"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_acceleratorMismatch)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"acceleratorMismatch", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for inconsistent casing or trailing punctuation, "
                                          "spaces, or newlines"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_transConsistency)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"transInconsistency", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Review fuzzy translations"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_fuzzyTranslations)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});

            wxStaticBoxSizer* pseudoTransSizer = new wxStaticBoxSizer(
                wxVERTICAL, poOptionsSizer->GetStaticBox(), _(L"Pseudo-translation"));

            wxBoxSizer* pseudoTransMethodSizer = new wxBoxSizer(wxHORIZONTAL);

            pseudoTransMethodSizer->Add(new wxStaticText(pseudoTransSizer->GetStaticBox(),
                                                         wxID_STATIC, _(L"Method:"),
                                                         wxDefaultPosition, wxDefaultSize),
                                        wxSizerFlags{}.CenterVertical());

            wxArrayString pseudoOptions;
            pseudoOptions.Add(_(L"None (do not generate anything)"));
            pseudoOptions.Add(_(L"UPPERCASE"));
            pseudoOptions.Add(_(L"European characters"));
            pseudoOptions.Add(_(L"Fill with 'X'es"));
            pseudoTransMethodSizer->Add(
                new wxChoice(pseudoTransSizer->GetStaticBox(), ID_PSEUDO_METHODS, wxDefaultPosition,
                             wxDefaultSize, pseudoOptions, 0,
                             wxGenericValidator(&m_pseudoTranslationMethod)),
                wxSizerFlags{}.Border(wxLEFT).Left().CenterVertical());

            pseudoTransSizer->Add(pseudoTransMethodSizer, wxSizerFlags{}.Expand().Border());

            m_pseudoSurroundingBracketsCheckbox = new wxCheckBox(
                pseudoTransSizer->GetStaticBox(), wxID_ANY, _(L"Add surrounding brackets"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_addPseudoTransBrackets));
            m_pseudoSurroundingBracketsCheckbox->Enable(m_pseudoTranslationMethod != 0);
            pseudoTransSizer->Add(m_pseudoSurroundingBracketsCheckbox,
                                  wxSizerFlags{}.Expand().Border());

            m_pseudoTrackCheckbox = new wxCheckBox(
                pseudoTransSizer->GetStaticBox(), wxID_ANY, _(L"Add tracking IDs"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_pseudoTrack));
            m_pseudoTrackCheckbox->Enable(m_pseudoTranslationMethod != 0);
            pseudoTransSizer->Add(m_pseudoTrackCheckbox, wxSizerFlags{}.Expand().Border());

            wxBoxSizer* pseudoWidthSizer = new wxBoxSizer(wxHORIZONTAL);

            m_pseudoSliderLabel =
                new wxStaticText(pseudoTransSizer->GetStaticBox(), wxID_STATIC,
                                 _(L"Increase width:"), wxDefaultPosition, wxDefaultSize);

            m_pseudoIncreaseSlider = new wxSlider(
                pseudoTransSizer->GetStaticBox(), wxID_ANY, m_widthPseudoIncrease, 0, 100,
                wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_MIN_MAX_LABELS,
                wxGenericValidator(&m_widthPseudoIncrease));

            m_pseudoSliderPercentLabel =
                new wxStaticText(pseudoTransSizer->GetStaticBox(), wxID_STATIC, L"%",
                                 wxDefaultPosition, wxDefaultSize);

            pseudoWidthSizer->Add(m_pseudoSliderLabel, wxSizerFlags{}.Left().CenterVertical());
            pseudoWidthSizer->Add(m_pseudoIncreaseSlider,
                                  wxSizerFlags{ 1 }.Expand().Border(wxLEFT));
            pseudoWidthSizer->Add(m_pseudoSliderPercentLabel,
                                  wxSizerFlags{}.Border(wxLEFT).Left().CenterVertical());

            pseudoTransSizer->Add(pseudoWidthSizer, wxSizerFlags{}.Expand().Border());

            poOptionsSizer->Add(gbSizer, wxSizerFlags{}.Expand().Border());
            poOptionsSizer->Add(pseudoTransSizer, wxSizerFlags{}.Expand().Border());
            }

        wxStaticBoxSizer* rcOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, transPage, _(L"Windows RC files"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(new wxCheckBox(rcOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for font issues"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_fontIssue)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"fontIssue", rcOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});
            rcOptionsSizer->Add(gbSizer, wxSizerFlags{}.Expand().Border());
            }

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(poOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->Add(rcOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->AddStretchSpacer();
        mainSizer->Add(
            new wxStaticText(transPage, wxID_STATIC,
                             _(L"Note: additional checks will be performed on source messages.")),
            wxSizerFlags{}.Expand().Border());

        transPage->SetSizer(mainSizer);
        listBook->AddPage(transPage, _(L"Resource Files"), false, 2);
        }

        // extra checks
        {
        wxPanel* extraChecksPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxStaticBoxSizer* formattingOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, extraChecksPage, _(L"Formatting && encoding checks:"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Non-UTF8 encoded files"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_nonUTF8File)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"nonUTF8File", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"BOM/UTF-8 signatures"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_UTF8FileWithBOM)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"UTF8FileWithBOM", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Unencoded extended ASCII characters"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_unencodedExtASCII)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(
                buildCodeLabel(L"unencodedExtASCII", formattingOptionsSizer->GetStaticBox()),
                wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Trailing spaces"), wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_trailingSpaces)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"trailingSpaces", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Tabs"), wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_tabs)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"tabs", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Overly long lines"), wxDefaultPosition, wxDefaultSize,
                                        0, wxGenericValidator(&m_wideLine)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"wideLine", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Comments not starting with a space"), wxDefaultPosition,
                                        wxDefaultSize, 0,
                                        wxGenericValidator(&m_commentMissingSpace)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(
                buildCodeLabel(L"commentMissingSpace", formattingOptionsSizer->GetStaticBox()),
                wxGBPosition(currentRow++, 1), wxGBSpan{});

            formattingOptionsSizer->Add(gbSizer, wxSizerFlags{}.Expand().Border());
            }

        wxStaticBoxSizer* codeOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, extraChecksPage, _(L"Code checks:"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(new wxCheckBox(codeOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Hard-coded ID numbers"), wxDefaultPosition,
                                        wxDefaultSize, 0,
                                        wxGenericValidator(&m_numberAssignedToId)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"numberAssignedToId", codeOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(codeOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"ID variables assigned the same value"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_dupValAssignedToIds)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"dupValAssignedToIds", codeOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            codeOptionsSizer->Add(gbSizer, wxSizerFlags{}.Border());
            }

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(formattingOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->Add(codeOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->AddStretchSpacer();

        extraChecksPage->SetSizer(mainSizer);
        listBook->AddPage(extraChecksPage, _(L"Additional Checks"), false, 3);
        }

    mainDlgSizer->Add(listBook, wxSizerFlags{ 1 }.Expand().Border());
    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL),
                      wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainDlgSizer);
    }
