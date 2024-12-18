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

#ifndef __INSERT_TRANS_COMMENT_DLG_H__
#define __INSERT_TRANS_COMMENT_DLG_H__

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
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/wx.h>

/** @brief Prompt for inserting a translator comment.*/
class InsertTransCommentDlg final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param id The window ID.
        @param caption The title of the export dialog.
        @param pos The screen position of the window.
        @param size The window size.
        @param style The window style (i.e., decorations and flags).*/
    explicit InsertTransCommentDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                                   const wxString& caption = _(L"Insert Translator Comment"),
                                   const wxPoint& pos = wxDefaultPosition,
                                   const wxSize& size = wxDefaultSize,
                                   long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                                wxRESIZE_BORDER);
    /// @private
    InsertTransCommentDlg(const InsertTransCommentDlg&) = delete;
    /// @private
    InsertTransCommentDlg& operator=(const InsertTransCommentDlg&) = delete;

    /// @returns The fully formatted results.
    [[nodiscard]]
    wxString GetFormattedOutput();

    /// @returns @c true if the selected comment is multiline.
    [[nodiscard]]
    bool IsMultilineComment()
        {
        TransferDataFromWindow();
        return m_selectedTag.starts_with(L"/*");
        }

  private:
    void CreateControls();
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

    wxString m_selectedTag;
    wxString m_comment;

    wxArrayString m_translatorTags;
    };

    /** @}*/

#endif //__INSERT_TRANS_COMMENT_DLG_H__
