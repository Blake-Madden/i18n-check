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

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef __INSERT_TRANS_MACRO_DLG_H__
#define __INSERT_TRANS_MACRO_DLG_H__

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
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/wx.h>

enum class TransMacroType
    {
    MarkForTranslation,
    MarkForNoTranslation
    };

/** @brief Prompt for inserting a macro to mark a string for either
        translation or that it should not be translated.*/
class InsertTransMacroDlg final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param stringToFormat The original string to wrap in a function call.
        @param id The window ID.
        @param caption The title of the export dialog.
        @param macroType Which type of macro to insert.
        @param pos The screen position of the window.
        @param size The window size.
        @param style The window style (i.e., decorations and flags).*/
    explicit InsertTransMacroDlg(
        wxWindow* parent, wxString stringToFormat, wxWindowID id = wxID_ANY,
        const wxString& caption = _(L"Insert"),
        const TransMacroType macroType = TransMacroType::MarkForTranslation,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER);
    /// @private
    InsertTransMacroDlg(const InsertTransMacroDlg&) = delete;
    /// @private
    InsertTransMacroDlg& operator=(const InsertTransMacroDlg&) = delete;

    /// @returns The fully formatted results.
    [[nodiscard]]
    wxString GetFormattedOutput();

  private:
    void CreateControls();

    void EnableExtraControls();

    [[nodiscard]]
    bool RequiresContext(const wxString& macro)
        {
        // wxGetTranslation has a context param, but it is optional
        return (macro == L"wxTRANSLATE_IN_CONTEXT" || macro == L"wxGETTEXT_IN_CONTEXT" ||
                macro == L"QT_TRANSLATE_NOOP" || macro == L"QT_TRANSLATE_N_NOOP" ||
                // KDE docs call the first parameter of these functions context,
                // but they actually can much more expressive strings.
                macro == L"i18nc" || macro == L"ki18nc");
        }

    [[nodiscard]]
    bool RequiresLiteralString(const wxString& macro)
        {
        return (macro != L"wxGetTranslation");
        }

    [[nodiscard]]
    bool RequiresDomain(const wxString& macro)
        {
        return (macro == L"wxGetTranslation");
        }

    [[nodiscard]]
    bool RequiresComment(const wxString& macro)
        {
        // QObject::tr has a disambiguation (comment) param, but it is optional
        return (macro == L"QT_TRANSLATE_NOOP3" || macro == L"QT_TRANSLATE_N_NOOP3" ||
                macro == L"QCoreApplication::translate");
        }

    [[nodiscard]]
    bool CanIncludeComment(const wxString& macro)
        {
        return (macro == L"tr");
        }

    void OnOK([[maybe_unused]] wxCommandEvent&);

    void OnHelpClicked([[maybe_unused]] wxCommandEvent& event)
        {
        const wxString docPath = []()
        {
            if (wxFile::Exists(wxStandardPaths::Get().GetResourcesDir() +
                               wxFileName::GetPathSeparator() + L"quneiform.pdf"))
                {
                return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() +
                       L"quneiform.pdf";
                }
            return wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath() +
                   wxFileName::GetPathSeparator() + L"quneiform.pdf";
        }();
        if (wxFile::Exists(docPath))
            {
            wxLaunchDefaultApplication(docPath);
            }
        }

    void OnContextHelp([[maybe_unused]] wxHelpEvent& event)
        {
        wxCommandEvent cmd;
        OnHelpClicked(cmd);
        }

    wxString m_stringToFormat;

    wxString m_selectedMacro;
    wxString m_context;
    wxString m_domain;
    wxString m_comment;

    TransMacroType m_macroType{ TransMacroType::MarkForTranslation };

    wxStaticText* m_contextLabel{ nullptr };
    wxTextCtrl* m_contextEntry{ nullptr };
    wxChoice* m_contextChoice{ nullptr };

    wxStaticText* m_domainLabel{ nullptr };
    wxTextCtrl* m_domainEntry{ nullptr };

    wxStaticText* m_commentLabel{ nullptr };
    wxTextCtrl* m_commentEntry{ nullptr };

    wxArrayString m_transMacros;
    wxArrayString m_noTransMacros;
    wxArrayString m_dtContexts;

    constexpr static int ID_MACRO_COMBO = wxID_HIGHEST;
    };

    /** @}*/

#endif //__INSERT_TRANS_MACRO_DLG_H__
