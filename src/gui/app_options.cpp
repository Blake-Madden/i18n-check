/////////////////////////////////////////////////////////////////////////////
// Name:        appoptions.cpp
// Author:      Blake Madden
// Copyright:   (c) 2021-2024 Blake Madden
// License:     3-Clause BSD license
// SPDX-License-Identifier: BSD-3-Clause
/////////////////////////////////////////////////////////////////////////////

#include "app_options.h"

//------------------------------------------------------
void I18NOptions::Save(const wxString& filePath)
    {
    wxXmlDocument xmlDoc;

    wxXmlNode* root = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, L"i18n-check-settings");
    xmlDoc.SetRoot(root);

    wxXmlNode* node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"path");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_filePath));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"excluded-paths");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_excludedPaths));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"checks");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, std::to_wstring(m_options)));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"fuzzy-translations");
    node->AddChild(
        new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_fuzzyTranslations ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"log-messages-can-be-translated");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 m_logMessagesCanBeTranslated ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"allow-translating-punctuation-only-strings");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 m_allowTranslatingPunctuationOnlyStrings ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"exceptions-should-be-translatable");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 m_exceptionsShouldBeTranslatable ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"min-words-for-classifying-unavailable-string");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 std::to_wstring(m_minWordsForClassifyingUnavailableString)));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"min-cpp-version");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, std::to_wstring(m_minCppVersion)));

    if (!xmlDoc.Save(filePath))
        {
        wxMessageBox(_(L"Error saving project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        }
    }

//------------------------------------------------------
void I18NOptions::Load(const wxString& filePath)
    {
    wxXmlDocument xmlDoc;
    if (!xmlDoc.Load(filePath))
        {
        wxMessageBox(_(L"Error loading project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    if (xmlDoc.GetRoot()->GetName() != "i18n-check-settings")
        {
        wxMessageBox(_(L"Invalid project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    wxXmlNode* child = xmlDoc.GetRoot()->GetChildren();
    while (child != nullptr)
        {
        if (child->GetName() == L"path")
            {
            m_filePath = child->GetNodeContent();
            }
        else if (child->GetName() == L"excluded-paths")
            {
            m_excludedPaths = child->GetNodeContent();
            }
        else if (child->GetName() == L"checks")
            {
            const wxString intVal = child->GetNodeContent();
            intVal.ToInt(&m_options);
            }
        else if (child->GetName() == L"fuzzy-translations")
            {
            m_fuzzyTranslations = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"log-messages-can-be-translated")
            {
            m_logMessagesCanBeTranslated = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"allow-translating-punctuation-only-strings")
            {
            m_allowTranslatingPunctuationOnlyStrings = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"exceptions-should-be-translatable")
            {
            m_exceptionsShouldBeTranslatable = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"min-words-for-classifying-unavailable-string")
            {
            const wxString intVal = child->GetNodeContent();
            intVal.ToInt(&m_minWordsForClassifyingUnavailableString);
            }
        else if (child->GetName() == L"min-cpp-version")
            {
            const wxString intVal = child->GetNodeContent();
            intVal.ToInt(&m_minCppVersion);
            }

        child = child->GetNext();
        }
    }
