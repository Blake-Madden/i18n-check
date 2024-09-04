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
#include <wx/artprov.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/treebook.h>
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
        @param pos The screen position of the window.
        @param size The window size.
        @param style The window style (i.e., decorations and flags).*/
    NewProjectDialog(wxWindow* parent, wxWindowID id = wxID_ANY,
                     const wxString& caption = _(L"New Project"),
                     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER);
    /// @private
    NewProjectDialog(const NewProjectDialog&) = delete;
    /// @private
    NewProjectDialog& operator=(const NewProjectDialog&) = delete;

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
    const wxString& GetExcludedPath() const noexcept
        {
        return m_excludedPaths;
        }

    void SetExcludedPath(const wxString& path)
        {
        m_excludedPaths = path;
        TransferDataToWindow();
        }

    [[nodiscard]]
    i18n_check::review_style GetOptions() const
        {
        return static_cast<i18n_check::review_style>(m_options);
        }

    void SetOptions(const i18n_check::review_style style);

    [[nodiscard]]
    bool UseFuzzyTranslations() const
        {
        return m_fuzzyTranslations;
        }

    void UseFuzzyTranslations(const bool use)
        {
        m_fuzzyTranslations = use;
        TransferDataToWindow();
        }

    [[nodiscard]]
    bool LogMessagesCanBeTranslated() const
        {
        return m_logMessagesCanBeTranslated;
        }

    void LogMessagesCanBeTranslated(const bool use)
        {
        m_logMessagesCanBeTranslated = use;
        TransferDataToWindow();
        }

    [[nodiscard]]
    bool AllowTranslatingPunctuationOnlyStrings() const
        {
        return m_allowTranslatingPunctuationOnlyStrings;
        }

    void AllowTranslatingPunctuationOnlyStrings(const bool use)
        {
        m_allowTranslatingPunctuationOnlyStrings = use;
        TransferDataToWindow();
        }

    [[nodiscard]]
    bool ExceptionsShouldBeTranslatable() const
        {
        return m_exceptionsShouldBeTranslatable;
        }

    void ExceptionsShouldBeTranslatable(const bool use)
        {
        m_exceptionsShouldBeTranslatable = use;
        TransferDataToWindow();
        }

    [[nodiscard]]
    size_t MinWordsForClassifyingUnavailableString() const
        {
        return m_minWordsForClassifyingUnavailableString;
        }

    void MinWordsForClassifyingUnavailableString(const size_t use)
        {
        m_minWordsForClassifyingUnavailableString = use;
        TransferDataToWindow();
        }

    [[nodiscard]]
    size_t MinCppVersion() const
        {
        return m_minCppVersion;
        }

    void MinCppVersion(const size_t use)
        {
        switch (use)
            {
        case 11:
            m_minCppVersion = 0;
            break;
        case 14:
            m_minCppVersion = 1;
            break;
        case 17:
            m_minCppVersion = 2;
            break;
        case 20:
            m_minCppVersion = 3;
            break;
        case 23:
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
    void OnExcludedFolderButtonClick([[maybe_unused]] wxCommandEvent&);
    void OnExcludedFileButtonClick([[maybe_unused]] wxCommandEvent&);
    void OnOK([[maybe_unused]] wxCommandEvent&);

    constexpr static int ID_FOLDER_BROWSE_BUTTON = wxID_HIGHEST;
    constexpr static int ID_EXCLUDED_FOLDERS_BROWSE_BUTTON = ID_FOLDER_BROWSE_BUTTON + 1;
    constexpr static int ID_EXCLUDED_FILES_BROWSE_BUTTON = ID_EXCLUDED_FOLDERS_BROWSE_BUTTON + 1;
    wxString m_filePath;
    wxString m_excludedPaths;
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
    // formatting options
    bool m_trailingSpaces{ true };
    bool m_tabs{ true };
    bool m_wideLine{ true };
    bool m_commentMissingSpace{ true };

    // checks
    int m_options{ i18n_check::review_style::no_checks };
    };

    /** @}*/

#endif //__I18N_PROJECT_DLG_H__
