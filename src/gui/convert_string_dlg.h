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

#ifndef __CONVERT_DLG_H__
#define __CONVERT_DLG_H__

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

/** @brief Performs various string conversions.*/
class ConvertStringDlg final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param id The window ID.
        @param caption The title of the export dialog.
        @param pos The screen position of the window.
        @param size The window size.
        @param style The window style (i.e., decorations and flags).*/
    explicit ConvertStringDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                              const wxString& caption = _(L"Convert String"),
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                           wxRESIZE_BORDER);
    /// @private
    ConvertStringDlg(const ConvertStringDlg&) = delete;
    /// @private
    ConvertStringDlg& operator=(const ConvertStringDlg&) = delete;

  private:
    void CreateControls();

    void OnTextChanged([[maybe_unused]] wxCommandEvent& event);

    void OnHelpClicked([[maybe_unused]] wxCommandEvent& event)
        {
        const wxString docPath = []()
        {
            if (wxFile::Exists(wxStandardPaths::Get().GetResourcesDir() +
                               wxFileName::GetPathSeparator() + L"cuneiform.pdf"))
                {
                return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() +
                       L"cuneiform.pdf";
                }
            return wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath() +
                   wxFileName::GetPathSeparator() + L"cuneiform.pdf";
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

    constexpr static wxWindowID ID_SOURCE_TEXT = wxID_HIGHEST;
    constexpr static wxWindowID ID_SELECTIONS = wxID_HIGHEST + 1;

    wxTextCtrl* m_inputTextCtrl{ nullptr };
    wxTextCtrl* m_outputTextCtrl{ nullptr };

    wxString m_input;

    int m_selectedConversion{ 0 };
    };

    /** @}*/

#endif //__CONVERT_DLG_H__
