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

#include "insert_transmacro_dlg.h"

//-------------------------------------------------------------
InsertTransMacroDlg::InsertTransMacroDlg(
    wxWindow* parent, wxString stringToFormat, wxWindowID id /*= wxID_ANY*/,
    const wxString& caption /*= _(L"Insert")*/,
    const TransMacroType macroType /*= TransMacroType::MarkForTranslation*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    : m_stringToFormat(std::move(stringToFormat)), m_macroType(macroType)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    m_transMacros.Add(L"_");
    // wxWidgets
    m_transMacros.Add(L"wxTRANSLATE");
    m_transMacros.Add(L"wxTRANSLATE_IN_CONTEXT");
    m_transMacros.Add(L"wxGETTEXT_IN_CONTEXT");
    m_transMacros.Add(L"wxGetTranslation");
    // Qt
    m_transMacros.Add(L"tr");
    m_transMacros.Add(L"translate");
    m_transMacros.Add(L"QT_TR_NOOP");
    m_transMacros.Add(L"QT_TR_N_NOOP");
    m_transMacros.Add(L"QT_TRANSLATE_NOOP");
    m_transMacros.Add(L"QT_TRANSLATE_N_NOOP");
    m_transMacros.Add(L"QT_TRANSLATE_NOOP3");
    m_transMacros.Add(L"QT_TRANSLATE_N_NOOP3");

    m_noTransMacros.Add(L"_DT");
    m_noTransMacros.Add(L"DONTTRANSLATE");

    m_dtContexts.Add(L"DTExplanation::DebugMessage");
    m_dtContexts.Add(L"DTExplanation::LogMessage");
    m_dtContexts.Add(L"DTExplanation::ProperNoun");
    m_dtContexts.Add(L"DTExplanation::DirectQuote");
    m_dtContexts.Add(L"DTExplanation::FilePath");
    m_dtContexts.Add(L"DTExplanation::InternalKeyword");
    m_dtContexts.Add(L"DTExplanation::Command");
    m_dtContexts.Add(L"DTExplanation::SystemEntry");
    m_dtContexts.Add(L"DTExplanation::FormatString");
    m_dtContexts.Add(L"DTExplanation::Syntax");
    m_dtContexts.Add(L"DTExplanation::Constant");
    m_dtContexts.Add(L"DTExplanation::NoExplanation");
    m_dtContexts.Add(L"DTExplanation::FontName");

    // bind events
    Bind(wxEVT_HELP, &InsertTransMacroDlg::OnContextHelp, this);
    Bind(wxEVT_BUTTON, &InsertTransMacroDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_CHOICE, [this](wxCommandEvent&) { EnableExtraControls(); }, ID_MACRO_COMBO);

    CreateControls();
    Centre();
    }

//-------------------------------------------------------------
void InsertTransMacroDlg::CreateControls()
    {
    wxBoxSizer* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* macroComboSzr = new wxBoxSizer(wxHORIZONTAL);
    macroComboSzr->Add(new wxStaticText(this, wxID_STATIC,
                                        (m_macroType == TransMacroType::MarkForTranslation) ?
                                            _(L"Translation marking function:") :
                                            _(L"Non-translatable marking function:")),
                       wxSizerFlags{}.CenterVertical());

    m_selectedMacro = (m_macroType == TransMacroType::MarkForTranslation) ? L"_" : L"_DT";
    macroComboSzr->Add(new wxChoice(this, ID_MACRO_COMBO, wxDefaultPosition, wxDefaultSize,
                                    (m_macroType == TransMacroType::MarkForTranslation) ?
                                        m_transMacros :
                                        m_noTransMacros,
                                    0, wxGenericValidator(&m_selectedMacro)),
                       wxSizerFlags{}.Left().CenterVertical().Border(wxLEFT));
    mainDlgSizer->Add(macroComboSzr, wxSizerFlags{}.Expand().Border());

    wxBoxSizer* contextSzr = new wxBoxSizer(wxHORIZONTAL);
    m_contextLabel = new wxStaticText(this, wxID_STATIC,
                                      (m_macroType == TransMacroType::MarkForTranslation) ?
                                          _(L"Context for translators:") :
                                          _(L"Context for developers:"));
    contextSzr->Add(m_contextLabel, wxSizerFlags{}.CenterVertical());
    if (m_macroType == TransMacroType::MarkForTranslation)
        {
        m_contextEntry =
            new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxBORDER_THEME, wxGenericValidator(&m_context));
        contextSzr->Add(m_contextEntry, wxSizerFlags{ 1 }.Expand().Border(wxLEFT));
        }
    else
        {
        m_context = L"DTExplanation::NoExplanation";
        m_contextChoice = new wxChoice(this, ID_MACRO_COMBO, wxDefaultPosition, wxDefaultSize,
                                       m_dtContexts, 0, wxGenericValidator(&m_context));
        contextSzr->Add(m_contextChoice, wxSizerFlags{ 1 }.Expand().Border(wxLEFT));
        }
    mainDlgSizer->Add(contextSzr, wxSizerFlags{}.Expand().Border());

    if (m_macroType == TransMacroType::MarkForTranslation)
        {
        wxBoxSizer* domainSzr = new wxBoxSizer(wxHORIZONTAL);
        m_domainLabel = new wxStaticText(this, wxID_STATIC, _(L"Message catalog domain:"));
        domainSzr->Add(m_domainLabel, wxSizerFlags{}.CenterVertical());
        m_domainEntry = new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                                       wxBORDER_THEME, wxGenericValidator(&m_domain));
        domainSzr->Add(m_domainEntry, wxSizerFlags{ 1 }.Expand().Border(wxLEFT));
        mainDlgSizer->Add(domainSzr, wxSizerFlags{}.Expand().Border());
        }

    m_commentLabel = new wxStaticText(this, wxID_STATIC,
                                      (m_macroType == TransMacroType::MarkForTranslation) ?
                                          _(L"Explanation for translators:") :
                                          _(L"Explanation for developers:"));
    mainDlgSizer->Add(m_commentLabel, wxSizerFlags{}.Border());
    m_commentEntry =
        new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition, FromDIP(wxSize{ 500, 150 }),
                       wxTE_RICH2 | wxBORDER_THEME | wxTE_BESTWRAP, wxGenericValidator(&m_comment));
    mainDlgSizer->Add(m_commentEntry, wxSizerFlags{ 1 }.Expand().Border());

    if (m_macroType == TransMacroType::MarkForNoTranslation)
        {
        mainDlgSizer->Add(
            new wxStaticText(
                this, wxID_STATIC,
                _(L"Note: include 'donttranslate.h' into your project to define these functions.")),
            wxSizerFlags{}.Expand().Border());
        }

    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL),
                      wxSizerFlags{}.Expand().Border());

    TransferDataToWindow();

    EnableExtraControls();

    SetSizerAndFit(mainDlgSizer);
    }

//-------------------------------------------------------------
void InsertTransMacroDlg::OnOK([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();

    const auto isQuoted = [](const auto& str)
    {
        const bool startOK =
            str.starts_with(L'"') || str.starts_with(L"L\"") || str.starts_with(L"LR\"");
        const bool endOK = str.ends_with(L'"');
        return startOK && endOK;
    };

    // strip string macros that aren't needed
    if ((m_stringToFormat.starts_with(L"wxT(") || m_stringToFormat.starts_with(L"wxS(")) &&
        m_stringToFormat.ends_with(L")"))
        {
        m_stringToFormat.erase(0, 4);
        m_stringToFormat.erase(m_stringToFormat.length() - 1);
        }

    if (RequiresContext(m_selectedMacro) && m_context.empty())
        {
        wxMessageBox(wxString::Format(_(L"%s requires a context."), m_selectedMacro),
                     _(L"Missing Context"));
        return;
        }
    if (RequiresLiteralString(m_selectedMacro) && !isQuoted(m_stringToFormat))
        {
        wxMessageBox(wxString::Format(_(L"%s can only be used for literal (i.e., quoted) strings."),
                                      m_selectedMacro),
                     _(L"String Type Mismatch"));
        return;
        }
    if (!RequiresLiteralString(m_selectedMacro) && isQuoted(m_stringToFormat))
        {
        wxMessageBox(
            wxString::Format(_(L"%s should not be used for literal (i.e., quoted) strings."),
                             m_selectedMacro),
            _(L"String Type Mismatch"));
        return;
        }
    if (RequiresDomain(m_selectedMacro) && m_domain.empty())
        {
        wxMessageBox(wxString::Format(_(L"%s requires a domain."), m_selectedMacro),
                     _(L"Missing Domain"));
        return;
        }
    if (RequiresComment(m_selectedMacro) && m_comment.empty())
        {
        wxMessageBox(wxString::Format(_(L"%s requires a comment/disambiguation."), m_selectedMacro),
                     _(L"Missing Comment"));
        return;
        }

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
void InsertTransMacroDlg::EnableExtraControls()
    {
    TransferDataFromWindow();
    if (m_macroType == TransMacroType::MarkForTranslation)
        {
        m_contextLabel->Enable(RequiresContext(m_selectedMacro) || RequiresDomain(m_selectedMacro));
        m_contextEntry->Enable(RequiresContext(m_selectedMacro) || RequiresDomain(m_selectedMacro));
        m_domainLabel->Enable(RequiresDomain(m_selectedMacro));
        m_domainEntry->Enable(RequiresDomain(m_selectedMacro));
        m_commentLabel->Enable(RequiresComment(m_selectedMacro) ||
                               CanIncludeComment(m_selectedMacro));
        m_commentEntry->Enable(RequiresComment(m_selectedMacro) ||
                               CanIncludeComment(m_selectedMacro));
        }
    }

//-------------------------------------------------------------
wxString InsertTransMacroDlg::GetFormattedOutput()
    {
    TransferDataFromWindow();

    const wxString quoteStart{ (m_stringToFormat.starts_with(L'L') ? L"L\"" : L"\"") };

    if (m_macroType == TransMacroType::MarkForTranslation)
        {
        if (RequiresDomain(m_selectedMacro))
            {
            return m_selectedMacro + L"(" + m_stringToFormat + L", " + quoteStart + m_domain +
                   L"\", " + quoteStart + m_context + "\")";
            }
        else if (RequiresComment(m_selectedMacro))
            {
            return m_selectedMacro + L"(" + quoteStart + m_context + L"\", " + m_stringToFormat +
                   L", " + quoteStart + m_comment + "\")";
            }
        else if (RequiresContext(m_selectedMacro))
            {
            return m_selectedMacro + L"(" + quoteStart + m_context + L"\", " + m_stringToFormat +
                   ")";
            }
        else
            {
            // tr() can take an optional disambiguation
            if (m_selectedMacro == L"tr" && !m_comment.empty())
                {
                return m_selectedMacro + L"(" + m_stringToFormat + L", " + quoteStart + m_comment +
                       "\")";
                }
            else
                {
                return m_selectedMacro + L"(" + m_stringToFormat + ")";
                }
            }
        }
    else
        {
        if (!m_comment.empty())
            {
            return m_selectedMacro + L"(" + m_stringToFormat + L", " + m_context + L", " +
                   quoteStart + m_comment + "\")";
            }
        if (m_context != L"DTExplanation::NoExplanation")
            {
            return m_selectedMacro + L"(" + m_stringToFormat + L", " + m_context + L")";
            }
        return m_selectedMacro + L"(" + m_stringToFormat + L")";
        }
    }
