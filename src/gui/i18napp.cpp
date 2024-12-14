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

#include "i18napp.h"
#include <iostream>
#include <wx/uilocale.h>

wxIMPLEMENT_APP(I18NApp);

I18NArtProvider::I18NArtProvider()
    {
    // cppcheck-suppress useInitializationList
    m_idFileMap = { { wxART_FILE_OPEN, L"images/file-open.svg" },
                    { wxART_FILE_SAVE, L"images/file-save.svg" },
                    { L"ID_CSV", L"images/file-csv.svg" },
                    { wxART_NEW, L"images/document.svg" },
                    { wxART_REFRESH, L"images/reload.svg" },
                    { wxART_DELETE, L"images/delete.svg" },
                    { wxART_HELP, L"images/help.svg" },
                    { wxART_CUT, L"images/cut.svg" },
                    { wxART_COPY, L"images/copy.svg" },
                    { wxART_PASTE, L"images/paste.svg" },
                    { wxART_REDO, L"images/redo.svg" },
                    { wxART_UNDO, L"images/undo.svg" },
                    { L"ID_CONVERT_STRING", L"images/convert-text.svg" },
                    { L"ID_SELECT_ALL", L"images/select-all.svg" },
                    { L"ID_INSERT", L"images/insert.svg" },
                    { L"ID_INSERT_TRANSLATOR_COMMENT", L"images/comment.svg" },
                    { L"ID_CODE", L"images/code.svg" },
                    { L"ID_TRANSLATIONS", L"images/translations.svg" },
                    { L"ID_CONVERT_TO_ENCODED_UNICODE", L"images/unicode-encoded.svg" },
                    { L"ID_INSERT_DT", L"images/dt.svg" },
                    { L"ID_INSERT_GETTEXT", L"images/gettext.svg" },
                    { L"ID_CHECK", L"images/check.svg" },
                    { L"ID_FORMAT", L"images/format.svg" },
                    { L"ID_DEBUG", L"images/bug.svg" },
                    { L"ID_SETTINGS", L"images/project-settings.svg" },
                    { L"ID_ABOUT", L"images/app-logo.svg" } };
    }

//-------------------------------------------
wxBitmapBundle I18NArtProvider::CreateBitmapBundle(const wxArtID& id, const wxArtClient& client,
                                                   const wxSize& size)
    {
    const auto filePath = m_idFileMap.find(id);

    return (filePath != m_idFileMap.cend()) ? GetSVG(filePath->second) :
                                              wxArtProvider::CreateBitmapBundle(id, client, size);
    }

//-------------------------------------------------------
wxBitmapBundle I18NArtProvider::GetSVG(const wxString& path) const
    {
    const wxString imagePath = [&path]()
    {
        if (wxFile::Exists(wxStandardPaths::Get().GetResourcesDir() +
                           wxFileName::GetPathSeparator() + path))
            {
            return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + path;
            }
        return wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath() +
               wxFileName::GetPathSeparator() + path;
    }();

    // load bitmap from disk if a local file
    if (wxFile::Exists(imagePath))
        {
        wxASSERT_MSG(wxBitmapBundle::FromSVGFile(imagePath, wxSize(16, 16)).IsOk(),
                     L"Failed to load SVG icon!");

        wxVector<wxBitmap> bmps;
        bmps.push_back(
            wxBitmapBundle::FromSVGFile(imagePath, wxSize(16, 16)).GetBitmap(wxSize(16, 16)));
        bmps.push_back(
            wxBitmapBundle::FromSVGFile(imagePath, wxSize(32, 32)).GetBitmap(wxSize(32, 32)));
        bmps.push_back(
            wxBitmapBundle::FromSVGFile(imagePath, wxSize(64, 64)).GetBitmap(wxSize(64, 64)));
        bmps.push_back(
            wxBitmapBundle::FromSVGFile(imagePath, wxSize(128, 128)).GetBitmap(wxSize(128, 128)));

        return wxBitmapBundle::FromBitmaps(bmps);
        }

    return wxBitmapBundle{};
    }

//------------------------------------------------------
bool I18NApp::OnInit()
    {
    if (!wxApp::OnInit())
        {
        return false;
        }

    SetAppName(L"Cuneiform");

    wxArtProvider::Push(new I18NArtProvider);

    // load the settings
    const wxString appSettingFolderPath = wxStandardPaths::Get().GetUserDataDir();
    if (!wxFileName::DirExists(appSettingFolderPath))
        {
        wxFileName::Mkdir(appSettingFolderPath);
        }
    m_optionsFilePath = appSettingFolderPath + wxFileName::GetPathSeparator() + L"settings.xml";
    if (wxFileName::FileExists(m_optionsFilePath))
        {
        m_defaultOptions.Load(m_optionsFilePath);
        }

    // set the locale (for number formatting, etc.) and load any translations
    wxUILocale::UseDefault();

    wxTranslations* const translations{ new wxTranslations{} };
    wxTranslations::Set(translations);
    if (!translations->AddCatalog(GetAppName()))
        {
        wxLogDebug(L"Could not find application's translation catalog.");
        }
    if (!translations->AddStdCatalog())
        {
        wxLogDebug(L"Could not find standard translation catalog.");
        }

#ifdef __WXMSW__
    MSWEnableDarkMode();
#endif

    // create the main application window
    I18NFrame* frame = new I18NFrame(GetAppName());
    if (m_defaultOptions.m_windowMaximized)
        {
        frame->Maximize();
        }
    frame->SetSize(frame->FromDIP(m_defaultOptions.m_windowSize));
    frame->InitControls();
    frame->CenterOnScreen();
    frame->Show(true);

    wxIcon appIcon;
    appIcon.CopyFromBitmap(
        wxArtProvider::GetBitmap(L"ID_ABOUT", wxART_OTHER, frame->FromDIP(wxSize{ 32, 32 })));
    frame->SetIcon(appIcon);

    wxFontEnumerator fe;
    auto fontNames = fe.GetFacenames();
    for (const auto& fn : fontNames)
        {
        // don't ignore font names that might also be real words
        if (fn.CmpNoCase(_DT(L"Symbol")) == 0 || fn.CmpNoCase(_DT(L"Modern")) == 0)
            {
            continue;
            }
        i18n_check::i18n_review::add_font_name_to_ignore(fn.wc_str());
        }

    return true;
    }

//------------------------------------------------------
int I18NApp::OnExit()
    {
    m_defaultOptions.Save(m_optionsFilePath);

    return 0;
    }
