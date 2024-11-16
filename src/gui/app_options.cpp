/********************************************************************************
 * Copyright (c) 2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#include "app_options.h"

//------------------------------------------------------
void I18NOptions::Save(const wxString& filePath)
    {
    wxXmlDocument xmlDoc;

    wxXmlNode* root = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, L"cuneiform-settings");
    xmlDoc.SetRoot(root);

    wxXmlNode* node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"path");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_filePath));

    auto* exclPathsNode = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"excluded-paths");
    for (const auto& exclFile : m_excludedPaths)
        {
        if (!exclFile.empty())
            {
            auto* pathNode = new wxXmlNode(exclPathsNode, wxXML_ELEMENT_NODE, L"excluded-path");
            pathNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, exclFile));
            }
        }

    auto* ignoredVarsNode = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"ignored-variables");
    for (const auto& ignoredVar : m_varsToIgnore)
        {
        if (!ignoredVar.empty())
            {
            auto* pathNode =
                new wxXmlNode(ignoredVarsNode, wxXML_ELEMENT_NODE, L"ignored-variable");
            pathNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, ignoredVar));
            }
        }

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"checks");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, std::to_wstring(m_options)));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"fuzzy-translations");
    node->AddChild(
        new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_fuzzyTranslations ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"pseudo-translation-method");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{},
                                 std::to_wstring(static_cast<int>(m_pseudoTranslationMethod))));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"pseudo-translation-add-brackets");
    node->AddChild(
        new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_addPseudoTransBrackets ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"pseudo-track");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_pseudoTrack ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"pseudo-width-increase");
    node->AddChild(
        new wxXmlNode(wxXML_TEXT_NODE, wxString{}, std::to_wstring(m_widthPseudoIncrease)));

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

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"window-maximized");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, m_windowMaximized ? L"true" : L"false"));

    node = new wxXmlNode(root, wxXML_ELEMENT_NODE, L"editor-height");
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxString{}, std::to_wstring(m_editorHeight)));

    if (!xmlDoc.Save(filePath))
        {
        wxMessageBox(_(L"Error saving project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        }
    }

//------------------------------------------------------
void I18NOptions::Load(const wxString& filePath)
    {
    // reset options
    m_filePath.clear();
    m_excludedPaths.clear();
    m_varsToIgnore.clear();
    m_options = static_cast<int64_t>(i18n_check::review_style::check_l10n_strings |
                                     i18n_check::review_style::check_suspect_l10n_string_usage |
                                     i18n_check::review_style::check_not_available_for_l10n |
                                     i18n_check::review_style::check_deprecated_macros |
                                     i18n_check::review_style::check_utf8_encoded |
                                     i18n_check::review_style::check_printf_single_number |
                                     i18n_check::review_style::check_l10n_contains_url |
                                     i18n_check::review_style::check_malformed_strings |
                                     i18n_check::review_style::check_fonts |
                                     i18n_check::review_style::all_l10n_checks);
    m_fuzzyTranslations = false;
    m_pseudoTranslationMethod = i18n_check::pseudo_translation_method::none;
    m_addPseudoTransBrackets = false;
    m_pseudoTrack = false;
    m_logMessagesCanBeTranslated = true;
    m_allowTranslatingPunctuationOnlyStrings = false;
    m_exceptionsShouldBeTranslatable = true;
    m_widthPseudoIncrease = 0;
    m_minWordsForClassifyingUnavailableString = 2;
    m_minCppVersion = 2014;

    wxXmlDocument xmlDoc;
    if (!xmlDoc.Load(filePath))
        {
        wxMessageBox(_(L"Error loading project file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    if (xmlDoc.GetRoot()->GetName() != "cuneiform-settings")
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
            wxXmlNode* excludedChild = child->GetChildren();
            while (excludedChild != nullptr)
                {
                if (!excludedChild->GetNodeContent().empty())
                    {
                    m_excludedPaths.push_back(excludedChild->GetNodeContent());
                    }
                excludedChild = excludedChild->GetNext();
                }
            }
        else if (child->GetName() == L"ignored-variables")
            {
            wxXmlNode* ignoredVarChild = child->GetChildren();
            while (ignoredVarChild != nullptr)
                {
                if (!ignoredVarChild->GetNodeContent().empty())
                    {
                    m_varsToIgnore.push_back(ignoredVarChild->GetNodeContent());
                    }
                ignoredVarChild = ignoredVarChild->GetNext();
                }
            }
        else if (child->GetName() == L"checks")
            {
            wxLongLong_t longLongVal{ 0 };
            child->GetNodeContent().ToLongLong(&longLongVal);
            m_options = static_cast<decltype(m_options)>(longLongVal);
            }
        else if (child->GetName() == L"fuzzy-translations")
            {
            m_fuzzyTranslations = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"pseudo-translation-method")
            {
            int tempInt{ 0 };
            child->GetNodeContent().ToInt(&tempInt);
            m_pseudoTranslationMethod = static_cast<i18n_check::pseudo_translation_method>(tempInt);
            }
        else if (child->GetName() == L"pseudo-translation-add-brackets")
            {
            m_addPseudoTransBrackets = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"pseudo-track")
            {
            m_pseudoTrack = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"pseudo-width-increase")
            {
            child->GetNodeContent().ToInt(&m_widthPseudoIncrease);
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
            child->GetNodeContent().ToInt(&m_minWordsForClassifyingUnavailableString);
            }
        else if (child->GetName() == L"min-cpp-version")
            {
            child->GetNodeContent().ToInt(&m_minCppVersion);
            }
        else if (child->GetName() == L"window-maximized")
            {
            m_windowMaximized = (child->GetNodeContent() == L"true");
            }
        else if (child->GetName() == L"editor-height")
            {
            child->GetNodeContent().ToInt(&m_editorHeight);
            }

        child = child->GetNext();
        }
    }
