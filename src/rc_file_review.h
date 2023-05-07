/** @addtogroup Internationalization
    @brief i18n classes.
    @date 2021-2023
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __RC_FILE_REVIEW_H__
#define __RC_FILE_REVIEW_H__

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include "i18n_review.h"

/// @brief Class for reviewing a Microsoft Windows resource file (*.RC).
namespace i18n_check
    {
    /** @brief Class to extract and review localizable/nonlocalizable text from C++ source code.*/
    class rc_file_review : public i18n_review
        {
    public:
        /** @brief Main interface for extracting resource text from C++ source code.
            @param cpp_text The C++ code text to extract text from.
            @param text_length The length of the text.
            @param file_name The (optional) name of source file being analyzed.*/
        void operator()(const std::wstring_view rcFileText,
                        const std::wstring& file_name = L"") final;
        };
    }

/** @}*/

#endif // __RC_FILE_REVIEW_H__
