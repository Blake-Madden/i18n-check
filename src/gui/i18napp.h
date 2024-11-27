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

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef __I18N_APP_H__
#define __I18N_APP_H__

#include "i18nframe.h"

//------------------------------------------------------
class I18NArtProvider final : public wxArtProvider
    {
  public:
    I18NArtProvider();

  protected:
    [[nodiscard]]
    wxBitmapBundle CreateBitmapBundle(const wxArtID& id, const wxArtClient& client,
                                      const wxSize& size) final;

    [[nodiscard]]
    wxBitmapBundle GetSVG(const wxString& path) const;

  private:
    std::map<wxArtID, wxString> m_idFileMap;
    };

// Application
class I18NApp : public wxApp
    {
  public:
    bool OnInit() final;
    int OnExit() final;

    I18NOptions m_defaultOptions;
    wxString m_optionsFilePath;
    };

    /** @}*/

#endif //__I18N_APP_H__
