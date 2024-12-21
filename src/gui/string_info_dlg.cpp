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

#include "string_info_dlg.h"

//-------------------------------------------------------------
StringInfoDlg::StringInfoDlg(
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
    const wxString& caption /*= _(L"Insert Translator Comment")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    // bind events
    Bind(wxEVT_HELP, &StringInfoDlg::OnContextHelp, this);
    Bind(wxEVT_BUTTON, &StringInfoDlg::OnHelpClicked, this, wxID_HELP);
    Bind(wxEVT_TEXT, &StringInfoDlg::OnTextChanged, this, ID_SOURCE_TEXT);

    CreateControls();
    Centre();
    }

//-------------------------------------------------------------
void StringInfoDlg::CreateControls()
    {
    wxBoxSizer* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

    mainDlgSizer->Add(new wxTextCtrl(this, ID_SOURCE_TEXT, wxString{}, wxDefaultPosition,
                                     FromDIP(wxSize{ 500, 150 }),
                                     wxTE_RICH2 | wxTE_MULTILINE | wxBORDER_THEME | wxTE_BESTWRAP,
                                     wxGenericValidator(&m_input)),
                      wxSizerFlags{ 1 }.Expand().Border());

    m_infoLabel = new wxStaticText(this, wxID_STATIC, wxString{});
    mainDlgSizer->Add(m_infoLabel, wxSizerFlags{}.Border());

    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxCLOSE | wxHELP),
                      wxSizerFlags{}.Expand().Border());

    TransferDataToWindow();

    SetSizerAndFit(mainDlgSizer);
    }

//-------------------------------------------------------------
void StringInfoDlg::OnTextChanged([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    const wxString strToReview{ m_input };
    wxString extAsciiValues;
    std::wstringstream encoded;
    for (const auto& chr : strToReview)
        {
        if (chr > 127)
            {
            extAsciiValues += chr;
            }
        encoded << LR"(\U)" << std::setfill(L'0') << std::setw(8) << std::uppercase << std::hex
                << static_cast<int>(chr);
        }
    wxString msg(wxString::Format(_(L"Length: %zu"), strToReview.length()));
    if (!extAsciiValues.empty())
        {
        msg.append(L"\n").append(wxString::Format(
            _(L"Contains extended ASCII characters: Yes (%s)"), extAsciiValues));
        // show the underlying hex values if not too long
        if (strToReview.length() < 16)
            {
            msg.append(L"\n").append(wxString::Format(_(L"Decoded: %s"), encoded.str()));
            }
        }
    else
        {
        msg.append(L"\n").append(_(L"Contains extended ASCII characters: No"));
        }

    if (m_infoLabel != nullptr)
        {
        m_infoLabel->SetLabel(msg);
        }
    }
