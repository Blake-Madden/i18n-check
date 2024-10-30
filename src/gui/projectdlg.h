/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2024
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __I18N_PROJECT_DLG_H__
#define __I18N_PROJECT_DLG_H__

#include "../i18n_review.h"
#include "app_options.h"
#include <wx/artprov.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/editlbox.h>
#include <wx/filename.h>
#include <wx/gbsizer.h>
#include <wx/listbook.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/wx.h>

/** @brief Prompt for selecting a folder,
        a file filter for files to select from it,
        and whether the search should be recursive.*/
class NewProjectDialog final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param id The window ID.
        @param caption The title of the export dialog.
        @param showFileOptions Whether to show the file and folder selection entries.
        @param pos The screen position of the window.
        @param size The window size.
        @param style The window style (i.e., decorations and flags).*/
    explicit NewProjectDialog(wxWindow* parent, wxWindowID id = wxID_ANY,
                              const wxString& caption = _(L"New Project"),
                              const bool showFileOptions = true,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                           wxRESIZE_BORDER);
    /// @private
    NewProjectDialog(const NewProjectDialog&) = delete;
    /// @private
    NewProjectDialog& operator=(const NewProjectDialog&) = delete;

    [[nodiscard]]
    I18NOptions GetAllOptions() noexcept
        {
        TransferDataFromWindow();
        I18NOptions options;
        options.m_options = GetOptions();
        options.m_filePath = GetPath();
        options.m_excludedPaths = GetExcludedPath();
        options.m_varsToIgnore = m_varsToIgnore;
        options.m_fuzzyTranslations = UseFuzzyTranslations();
        options.m_addPseudoTransBrackets = m_addPseudoTransBrackets;
        options.m_pseudoTrack = m_pseudoTrack;
        options.m_widthPseudoIncrease = m_widthPseudoIncrease;
        options.m_pseudoTranslationMethod =
            static_cast<i18n_check::pseudo_translation_method>(m_pseudoTranslationMethod);
        options.m_logMessagesCanBeTranslated = LogMessagesCanBeTranslated();
        options.m_allowTranslatingPunctuationOnlyStrings = AllowTranslatingPunctuationOnlyStrings();
        options.m_exceptionsShouldBeTranslatable = ExceptionsShouldBeTranslatable();
        options.m_minWordsForClassifyingUnavailableString =
            MinWordsForClassifyingUnavailableString();
        options.m_minCppVersion = MinCppVersion();
        return options;
        }

    void SetAllOptions(const I18NOptions& options)
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
        m_minWordsForClassifyingUnavailableString =
            options.m_minWordsForClassifyingUnavailableString;
        MinCppVersion(options.m_minCppVersion);
        TransferDataToWindow();
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
        }

    /// @returns The path of the selected folder.
    [[nodiscard]]
    const wxString& GetPath() const noexcept
        {
        return m_filePath;
        }

    void SetPath(const wxString& path)
        {
        m_filePath = path;
        TransferDataToWindow();
        }

    [[nodiscard]]
    const wxArrayString& GetExcludedPath() const noexcept
        {
        return m_excludedPaths;
        }

    [[nodiscard]]
    const wxArrayString& GetIgnoreVariables() const noexcept
        {
        return m_varsToIgnore;
        }

    [[nodiscard]]
    i18n_check::review_style GetOptions() const noexcept
        {
        return static_cast<i18n_check::review_style>(m_options);
        }

    void SetOptions(const i18n_check::review_style style);

    [[nodiscard]]
    bool UseFuzzyTranslations() const noexcept
        {
        return m_fuzzyTranslations;
        }

    void UseFuzzyTranslations(const bool useFuzzy)
        {
        m_fuzzyTranslations = useFuzzy;
        TransferDataToWindow();
        }

    [[nodiscard]]
    bool LogMessagesCanBeTranslated() const noexcept
        {
        return m_logMessagesCanBeTranslated;
        }

    void LogMessagesCanBeTranslated(const bool logTran)
        {
        m_logMessagesCanBeTranslated = logTran;
        TransferDataToWindow();
        }

    [[nodiscard]]
    bool AllowTranslatingPunctuationOnlyStrings() const noexcept
        {
        return m_allowTranslatingPunctuationOnlyStrings;
        }

    void AllowTranslatingPunctuationOnlyStrings(const bool allow)
        {
        m_allowTranslatingPunctuationOnlyStrings = allow;
        TransferDataToWindow();
        }

    [[nodiscard]]
    bool ExceptionsShouldBeTranslatable() const noexcept
        {
        return m_exceptionsShouldBeTranslatable;
        }

    void ExceptionsShouldBeTranslatable(const bool exptTran)
        {
        m_exceptionsShouldBeTranslatable = exptTran;
        TransferDataToWindow();
        }

    [[nodiscard]]
    size_t MinWordsForClassifyingUnavailableString() const noexcept
        {
        return m_minWordsForClassifyingUnavailableString;
        }

    void MinWordsForClassifyingUnavailableString(const size_t minWords)
        {
        m_minWordsForClassifyingUnavailableString = minWords;
        TransferDataToWindow();
        }

    [[nodiscard]]
    size_t MinCppVersion() const
        {
        switch (m_minCppVersion)
            {
        case 0:
            return 2011;
        case 1:
            return 2014;
        case 2:
            return 2017;
        case 3:
            return 2020;
        case 4:
            return 2023;
            }
        return m_minCppVersion;
        }

    void MinCppVersion(const size_t minVer)
        {
        switch (minVer)
            {
        case 2011:
            m_minCppVersion = 0;
            break;
        case 2014:
            m_minCppVersion = 1;
            break;
        case 2017:
            m_minCppVersion = 2;
            break;
        case 2020:
            m_minCppVersion = 3;
            break;
        case 2023:
            m_minCppVersion = 4;
            break;
        default:
            m_minCppVersion = 1;
            };
        TransferDataToWindow();
        }

  private:
    void CreateControls();
    void OnFolderButtonClick([[maybe_unused]] wxCommandEvent&);
    void OnFileButtonClick([[maybe_unused]] wxCommandEvent&);
    void OnExcludedFolderButtonClick([[maybe_unused]] wxCommandEvent&);
    void OnExcludedFileButtonClick([[maybe_unused]] wxCommandEvent&);
    void OnOK([[maybe_unused]] wxCommandEvent&);

    constexpr static int ID_FOLDER_BROWSE_BUTTON = wxID_HIGHEST;
    constexpr static int ID_FILE_BROWSE_BUTTON = wxID_HIGHEST + 1;
    constexpr static int ID_EXCLUDED_FOLDERS_BROWSE_BUTTON = wxID_HIGHEST + 2;
    constexpr static int ID_EXCLUDED_FILES_BROWSE_BUTTON = wxID_HIGHEST + 3;
    constexpr static int ID_PSEUDO_METHODS = wxID_HIGHEST + 4;

    bool m_showFileOptions{ true };

    wxString m_filePath;
    wxArrayString m_excludedPaths;
    wxArrayString m_varsToIgnore;
    // options for all file types
    bool m_notL10NAvailable{ true };
    bool m_suspectL10NString{ true };
    bool m_suspectL10NUsage{ true };
    bool m_printfMismatch{ true };
    bool m_urlInL10NString{ true };
    bool m_deprecatedMacro{ true };
    bool m_printfSingleNumber{ true };
    bool m_malformedString{ true };
    bool m_logMessagesCanBeTranslated{ true };
    bool m_allowTranslatingPunctuationOnlyStrings{ true };
    bool m_exceptionsShouldBeTranslatable{ true };
    int m_minWordsForClassifyingUnavailableString{ 2 };
    // C++ options
    bool m_nonUTF8File{ true };
    bool m_UTF8FileWithBOM{ true };
    bool m_unencodedExtASCII{ true };
    bool m_numberAssignedToId{ true };
    bool m_dupValAssignedToIds{ true };
    int m_minCppVersion{ 1 };
    // RC options
    bool m_fontIssue{ true };
    // PO options
    bool m_fuzzyTranslations{ true };
    bool m_pseudoTrack{ false };
    int m_pseudoTranslationMethod{ 0 };
    bool m_addPseudoTransBrackets{ false };
    int m_widthPseudoIncrease{ 0 };
    // formatting options
    bool m_trailingSpaces{ true };
    bool m_tabs{ true };
    bool m_wideLine{ true };
    bool m_commentMissingSpace{ true };
    // checks
    int64_t m_options{ i18n_check::review_style::no_checks };

    wxCheckBox* m_pseudoSurroundingBracketsCheckbox{ nullptr };
    wxCheckBox* m_pseudoTrackCheckbox{ nullptr };
    wxStaticText* m_pseudoSliderLabel{ nullptr };
    wxStaticText* m_pseudoSliderPercentLabel{ nullptr };
    wxSlider* m_pseudoIncreaseSlider{ nullptr };
    wxEditableListBox* m_exclusionList{ nullptr };
    wxEditableListBox* m_ignoredVarsList{ nullptr };
    };

    /** @}*/

#endif //__I18N_PROJECT_DLG_H__
