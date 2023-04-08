#include <iomanip>
#include <sstream>
#include <algorithm>
#include <set>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/cpp_i18n_review.h"

using namespace i18n_check;
using namespace Catch::Matchers;

TEST_CASE("CPP Tests", "[cpp]")
    {
    SECTION("Null")
        {
        cpp_i18n_review cpp;
        cpp(nullptr, 72);
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        const wchar_t* code = L"//comment\nif assert(true && \"this is an error\")";
        cpp(code, 0);
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Debug defined block")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(int Wisteria::UI::BaseApp::OnExit()
            {
            wxLogDebug(__WXFUNCTION__);
            SaveFileHistoryMenu();
            wxDELETE(m_docManager);

        #ifdef __WXMSW__
            #if wxDEBUG_LEVEL >= 2
                // dump max memory usage
                // https://docs.microsoft.com/en-us/windows/win32/psapi/collecting-memory-usage-information-for-a-process?redirectedfrom=MSDN
                PROCESS_MEMORY_COUNTERS memCounter;
                ::ZeroMemory(&memCounter, sizeof(PROCESS_MEMORY_COUNTERS));
                if (::GetProcessMemoryInfo(::GetCurrentProcess(), &memCounter, sizeof(memCounter)))
                    {
                    const wxString memMsg = wxString::Format(L"Peak Memory Usage: %.02fGbs.",
                        safe_divide<double>(memCounter.PeakWorkingSetSize, 1024*1024*1024));
                    wxLogDebug(memMsg);
                    OutputDebugString(memMsg.wc_str());
                    }
            #elif
                const wxString memMsg = wxString::Format(L"Info: Peak Memory Usage: %.02fGbs.",
                        safe_divide<double>(memCounter.PeakWorkingSetSize, 1024*1024*1024));
                MsgBox(memMsg);
            #endif
            #ifdef _DEBUG
                MsgBox("Debug message 0!");
            #endif
            #ifndef NDEBUG
                MsgBox("Debug message 1!");
            #endif
            #ifndef NDEBUG
                MsgBox("Debug message 2!");
            #elif
                MsgBox("Release message!");
            #endif
            #if defined _DEBUG
                MsgBox("Debug message 3!");
            #endif
        #endif
            return wxApp::OnExit();
            })";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
            L"Info: Peak Memory Usage: %.02fGbs.");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string ==
            L"Release message!");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("XML Tag")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "<and or>";)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "<a href=\"#scores\">")";
        cpp.clear_results();
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        
        code = LR"(auto var = "<a href=\"#")";
        cpp.clear_results();
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "xml version=\"1.0\"")";
        cpp.clear_results();
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "xml version=\"1.0\">")";
        cpp.clear_results();
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "xml version=\"1.0\">Hello")";
        cpp.clear_results();
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);

        code = LR"(auto var = "< and or >";)";
        cpp.clear_results();
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Separated strings")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(MessageBox("This is a long "
                                             "message across "
                                             "multiple lines");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              L"This is a long message across multiple lines");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Separated strings int 64")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(MessageBox("The amount is %0" PRId64 "\n");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              LR"(The amount is %0\n)");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Separated strings bad int 64 macro")
        {
        cpp_i18n_review cpp;
        // "46" is wrong, so parse this as two strings
        const wchar_t* code = LR"(MessageBox("Invalid Likert response: %\n" PRIu46
                                             "Column: %s\nValues should not exceed 7.");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              LR"(Invalid Likert response: %\n)");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string ==
              LR"(Column: %s\nValues should not exceed 7.)");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("File filter")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"var = \"PNG (*.png)|*.png\"";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
              L"PNG (*.png)|*.png");
        }

    SECTION("File filter word")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"var = \"Bitmap (*.bmp)|*.bmp\"";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
              L"Bitmap (*.bmp)|*.bmp");
        }

    SECTION("File filter multi extensions")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"var = \"TIFF (*.tif;*.tiff)|*.tif;*.tiff\"";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
              L"TIFF (*.tif;*.tiff)|*.tif;*.tiff");
        }

    SECTION("Printf integers")
        {
        const wchar_t* code = LR"(auto = sprintf("%zu", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%d", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%+d", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%ll", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%s", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 0);
            }
        }

    SECTION("Printf floats")
        {
        const wchar_t* code = LR"(auto = sprintf("%f", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%lf", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%0.4f", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, std::wcslen(code));
            cpp.review_strings();
            // specific formatting, so std::to_string() can't replace this
            CHECK(cpp.get_printf_single_numbers().size() == 0);
            }
        }
    
    SECTION("Ignored macro")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxString GetName() const wxOVERRIDE { return wxT("Simple DirectMedia Layer"); })";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              L"Simple DirectMedia Layer");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Ignore email contact info")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = "Blake Madden <empty.name@company.mail.org>")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
            L"Blake Madden <empty.name@company.mail.org>");
        }

    SECTION("Ignore email contact info should not be l10n")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = _("Blake Madden <empty.name@company.mail.org>"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        }

    SECTION("Ignore email")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "emptyname@mail.org")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
            L"emptyname@mail.org");
        }

    SECTION("Ignore URL")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "www.company.com")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
            L"www.company.com");
        }

    SECTION("Ignore URL")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = _("Contact us at www.company.com"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        }

    SECTION("Internal file name")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"Log_Rep[o]rt-1.log", false));
        CHECK(cpp.is_untranslatable_string(L"c:\\Reports\\Log_Report-1.log", false));
        CHECK(cpp.is_untranslatable_string(L"c:\\src-stuff,files\\Log_Reporter.h", false));
        CHECK(cpp.is_untranslatable_string(L"c:\\users\\yam\\documents&files\\audacity\\mixer\\n\\audacity\\src\\dither.cpp", false));
        CHECK(cpp.is_untranslatable_string(L"/src/Log_Reporter.sps9", false));
        // double extensions (common on UNIX files)
        CHECK(cpp.is_untranslatable_string(L"dynlib.so.o", false));
        // file extension
        CHECK(cpp.is_untranslatable_string(L".sps9", false));
        CHECK(cpp.is_untranslatable_string(L"*.sps9", false)); // wild card
        CHECK(cpp.is_untranslatable_string(L"Log-Report-1.log", false));
        // ultra simple relative file path
        CHECK(cpp.is_untranslatable_string(L"shaders/player1.vert", false));
        CHECK(cpp.is_untranslatable_string(L"resources\\shaders\\player1.vert", false));
        // not really a file name, the ending is deceptively like a file extension
        CHECK_FALSE(cpp.is_untranslatable_string(L"The maximum number of notes must be in the range 1..128", false));
        }

    SECTION("Filename")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("another.hhp"));)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == L"another.hhp");
        }

    SECTION("Filename with folder path")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("Enums/Tests.h"));)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == L"Enums/Tests.h");

        CHECK(i18n_string_util::is_file_address(L"Enums/Tests.h", 13));
        }

    SECTION("Place holder")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("  Lorem ipsum dolor sit amet"));)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == L"  Lorem ipsum dolor sit amet");
        }
    
    SECTION("Long place holder")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque nisl \nmassa, luctus ut ligula vitae, suscipit tempus velit. Vivamus sodales, quam in \nconvallis posuere, libero nisi ultricies orci, nec lobortis.\n"));)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == LR"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque nisl \nmassa, luctus ut ligula vitae, suscipit tempus velit. Vivamus sodales, quam in \nconvallis posuere, libero nisi ultricies orci, nec lobortis.\n)");
        }

    SECTION("Min word count")
        {
        // default is to ignore strings with just one word
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("NETHEREALM");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"NETHEREALM");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);

        // now, complain about one-word strings not exposed for l10n
        cpp.clear_results();
        cpp.set_min_words_for_classifying_unavailable_string(1);
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"NETHEREALM");
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("Min word count apos")
        {
        // default is to ignore strings with just one word
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("NETHEREALM'S");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"NETHEREALM'S");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);

        // now, complain about one-word strings not exposed for l10n
        cpp.clear_results();
        cpp.set_min_words_for_classifying_unavailable_string(1);
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"NETHEREALM'S");
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("Min word count hypthen")
        {
        // default is to ignore strings with just one word
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("part-time");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"part-time");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);

        // now, complain about one-word strings not exposed for l10n
        cpp.clear_results();
        cpp.set_min_words_for_classifying_unavailable_string(1);
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"part-time");
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("Not filename too long")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("Microsoft Windows 2000 system level function loading error occurred during process initialization - Unable to load a function from the library file riched.dll");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"wxMessageBox");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Hex color")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"&	c #437A40", false));
        }

    SECTION("Xes")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L" xx xx x", false));
        }

    SECTION("HTML")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"<tt><span style = 'font-weight:bold;'>", false));
        CHECK(cpp.is_untranslatable_string(L"<table style=\\\"width:100%;\"><tr><td width=\"33%\">", false));
        CHECK(cpp.is_untranslatable_string(L"<p style=\\\"font-family: %s; font-size: %dpt; color: rgb(%u, %u, %u)\\\">\n", false));
        CHECK(cpp.is_untranslatable_string(LR"(<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n<head>\n<meta content=\"text/html; charset=UTF-8\"/>\n<title></title>\n<link href=\"stylesheet.css\" type=\"text/css\" rel=\"stylesheet\" />\n<link rel=\"stylesheet\" type=\"application/vnd.adobe-page-template+xml\" href=\"page-template.xpgt\"/>\n</head>)", false));
        CHECK(cpp.is_untranslatable_string(LR"(<br />&nbsp;&nbsp;&nbsp;&nbsp;&ldquo;<span style=\"font-style:italic;\">%s</span>&rdquo;)", false));
        }
    
    SECTION("SQL")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"INSERT into DATA", false));
        CHECK(cpp.is_untranslatable_string(L"select * FROM", false));
        }

    SECTION("Macro variable")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   ("Direct2D failed"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"DX_MSG"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("Function signature")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(void DefineProperty(const char *name, plcob pb, std::string description="my description"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"my description"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"description"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("Macro variable ignored")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   "Direct2D failed")";
        cpp.add_variable_name_pattern_to_ignore(std::wregex(L"DX_MSG"));
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == std::wstring(L"DX_MSG"));
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("Orphan string")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(if (value =="my message"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"my message"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L""));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::orphan);
        }

    SECTION("Macro variable expands to function")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   MessageBox("Direct2D failed"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"MessageBox"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::function);
        }

    SECTION("Allow punctuation only")
        {
        cpp_i18n_review cpp;
        cpp.allow_translating_punctuation_only_strings(false);
        CHECK(cpp.is_untranslatable_string(L" % ", false));
        cpp.allow_translating_punctuation_only_strings(true);
        CHECK_FALSE(cpp.is_untranslatable_string(L" % ", false));
        }

    SECTION("Windows names")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"Windows 3.1", false));
        CHECK(cpp.is_untranslatable_string(L"Windows 98", false));
        CHECK(cpp.is_untranslatable_string(L"Windows 2000", false));
        CHECK(cpp.is_untranslatable_string(L"Windows NT", false));
        CHECK(cpp.is_untranslatable_string(L"Windows NT SP4", false));
        CHECK(cpp.is_untranslatable_string(L"Windows XP", false));
        CHECK(cpp.is_untranslatable_string(L"Windows XP SP3", false));
        CHECK(cpp.is_untranslatable_string(L"Windows Server", false));
        CHECK(cpp.is_untranslatable_string(L"Windows Server 2012", false));
        CHECK(cpp.is_untranslatable_string(L"Windows Server 2012 R2", false));
        CHECK(cpp.is_untranslatable_string(L"Windows Vista", false));
        CHECK(cpp.is_untranslatable_string(L"Windows 8", false));
        CHECK(cpp.is_untranslatable_string(L"Windows 8.1", false));
        CHECK(cpp.is_untranslatable_string(L"Windows 10", false));
        }

    SECTION("Unix path")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"/usr/myfolder/libs", false));
        CHECK(cpp.is_untranslatable_string(L"/usr/myfolder/libs/", false));
        CHECK(cpp.is_untranslatable_string(L"/usr/myfolder/libs/info.so", false));
        CHECK(cpp.is_untranslatable_string(L"/usr/libs/info folder", false));
        // not enough slashes to make it appear like a file path
        CHECK_FALSE(cpp.is_untranslatable_string(L"/usr is a root folder", false));
        }

    SECTION("Escaped quotes")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = L"description.Replace(wxT(\"\\\\\\\"\"), wxT(\"/\"));";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 2);
        CHECK(cpp.get_internal_strings()[0].m_string == L"\\\\\\\"");
        CHECK(cpp.get_internal_strings()[1].m_string == L"/");
        cpp.clear_results();
        code = L"view = (mCurTrack[0]->GetWaveformSettings().scaleType == 0) ? wxT(\"\\\"Waveform\\\"\") : wxT(\"\\\"Waveform (dB)\\\"\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"\\\"Waveform\\\"");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"\\\"Waveform (dB)\\\"");
        }

    SECTION("Quote in single quotes")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = L"if (a == '\"')\nAddCheckBox(_(\"&Use legacy (version 3) syntax.\"),\n(mVersion == 3) ? wxT(\"true\") : wxT(\"false\"));";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"&Use legacy (version 3) syntax.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"true");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"false");
        }

    SECTION("Internal Strings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"", false));
        CHECK(cpp.is_untranslatable_string(L" ", false));
        CHECK(cpp.is_untranslatable_string(L"  \t", false));
        CHECK(cpp.is_untranslatable_string(L"1", false));
        CHECK(cpp.is_untranslatable_string(L"1.0", false));
        CHECK(cpp.is_untranslatable_string(L">", false));
        CHECK(cpp.is_untranslatable_string(L"> ", false));
        CHECK(cpp.is_untranslatable_string(L"\\n\\t\\r ", false));
        }

    SECTION("Parens")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"GetMenuBar()->SetLabel(XRCID(\"ID_SAVE_ITEM\"), wxString::Format(_(\"Export %s...\"), GetActiveProjectWindow()->GetName()) );";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"Export %s...");
        CHECK(cpp.get_internal_strings()[0].m_string == L"ID_SAVE_ITEM");
        }

    SECTION("Parens 2")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"customTestMenu->Append(XRCID(\"ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST\"), wxString::Format(_(\"Add Custom \\\"%s\\\"...\"), wxT(\"New Dale-Chall\")) );";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"Add Custom \\\"%s\\\"...");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"New Dale-Chall");
        CHECK(cpp.get_internal_strings()[0].m_string == L"ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST");
        }

    SECTION("Pascal Case")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"value = \"XmlHttpRequest\"; value = \"SupportsIpv6OnIos\"; value = \"Xml2HttpRequest\";";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 3);
        CHECK(cpp.get_internal_strings()[0].m_string == L"XmlHttpRequest");
        CHECK(cpp.get_internal_strings()[1].m_string == L"SupportsIpv6OnIos");
        CHECK(cpp.get_internal_strings()[2].m_string == L"Xml2HttpRequest");
        }

    SECTION("Camel Case")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"value = \"xmlHttpRequest\"; value = \"supportsIpv6OnIos\"; value = \"xml2HttpRequest\";";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 3);
        CHECK(cpp.get_internal_strings()[0].m_string == L"xmlHttpRequest");
        CHECK(cpp.get_internal_strings()[1].m_string == L"supportsIpv6OnIos");
        CHECK(cpp.get_internal_strings()[2].m_string == L"xml2HttpRequest");
        }

    SECTION("Internal CSS Strings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"font-style: italic;", false));
        CHECK(cpp.is_untranslatable_string(L"  font-style: italic;  ", false));
        CHECK(cpp.is_untranslatable_string(L"font-weight: bold", false));
        CHECK(cpp.is_untranslatable_string(L" color:red", false));
        CHECK(cpp.is_untranslatable_string(L"background-COLOR:red, false", false));
        CHECK(cpp.is_untranslatable_string(L" style = 'color: red", false));
        }

    SECTION("Internal HTML Strings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"<html>", false));
        CHECK(cpp.is_untranslatable_string(L"<!--commment>", false));
        CHECK(cpp.is_untranslatable_string(L"<SPAN style=", false));
        CHECK_FALSE(cpp.is_untranslatable_string(L"<HTML>hello", false));
        CHECK(cpp.is_untranslatable_string(LR"(<HTML = "hello"></html>)", false));
        CHECK(cpp.is_untranslatable_string(LR"(<a href="website"><br>)", false));
        CHECK(cpp.is_untranslatable_string(LR"(]]</center>\n)", false));
        CHECK(cpp.is_untranslatable_string(L"&amp;", false));
        CHECK(cpp.is_untranslatable_string(L"&#107;", false));
        CHECK(cpp.is_untranslatable_string(L"&#xF8;", false));
        CHECK(cpp.is_untranslatable_string(LR"(charset = \"%s\"\n)", false));
        }

    SECTION("Internal XML Strings")
        {
        cpp_i18n_review cpp;
        CHECK_FALSE(cpp.is_untranslatable_string(L"<?xml>hello", false));
        CHECK(cpp.is_untranslatable_string(L"<?XML>7</XML>", false));
        // generic tags
        CHECK(cpp.is_untranslatable_string(L"<doc-val>&entity;</doc-val>", false));
        CHECK(cpp.is_untranslatable_string(LR"(<comment =\")", false));
        CHECK(cpp.is_untranslatable_string(LR"(<startdoctype name=\"%s\")", false));
        CHECK(cpp.is_untranslatable_string(LR"(<image x=%d y=\"%d\" width = '%dpx' height=\"%dpx\")", false));
        }

    SECTION("Custom XML")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"<ice>", false));
        CHECK(cpp.is_untranslatable_string(L"</ice>", false));
        CHECK(cpp.is_untranslatable_string(L"<ice> 9, </ice>", false));
        CHECK(cpp.is_untranslatable_string(L"<>", false));
        CHECK(cpp.is_untranslatable_string(L"<ice><ice>", false));
        CHECK(cpp.is_untranslatable_string(L"<ice-level><ice-level>", false));
        CHECK(cpp.is_untranslatable_string(L"<ice> <ice>", false));
        CHECK(cpp.is_untranslatable_string(L"<unrecognized version=\"3\">", false));
        }

    SECTION("Formulas")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"=color", false));
        CHECK(cpp.is_untranslatable_string(L"=small", false));
        CHECK(cpp.is_untranslatable_string(L"Open()", false));
        CHECK(cpp.is_untranslatable_string(L"ABS(-2.7)", false));
        CHECK(cpp.is_untranslatable_string(L"POW(-4, 2)", false));
        CHECK(cpp.is_untranslatable_string(L"SUM(5,6)", false));
        CHECK(cpp.is_untranslatable_string(L"SUM(5;6)", false));
        CHECK(cpp.is_untranslatable_string(L"SUM(R[-4]C:R[-1]C)", false));
        CHECK(cpp.is_untranslatable_string(L"=SUM(R[-4]C:R[-1]C)", false));
        }

    SECTION("Variable name define")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(#define SAVE_MAGIC_V1 "fish")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"fish");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"SAVE_MAGIC_V1"));
        }

    SECTION("__asm")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"__asm int \"A\"\n  __asm {int \"B\"\nint \"C\"}\n\nif b__asm(\"this is an error\")__asm";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("asm")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"asm int \"A\"\n  asm (int \"B\"\nint \"C\")\n\nif basm(\"this is an error\")asm";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("asm volatile")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(asm volatile (                            \
                    "sd  %[val_m],  %[pdst_sd_m]  \n\t"   \
                                                          \
                    : [pdst_sd_m] "=m" (*pdst_sd_m)       \
                    : [val_m] "r" (val_m)                 \
                );if basm("this is an error"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("__asm__")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(__asm__ __volatile__ (
              "cpuid":
           "=a" (CPUInfo[0]),
              "=b" (CPUInfo[1]),
              "=c" (CPUInfo[2]),
              "=d" (CPUInfo[3]) :
           "a" (InfoType)
              );if basm("this is an error"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("__asm__ inline")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define CATCH_TRAP()  __asm__(".inst 0xde01"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Preprocessor Defined Variable In String Helper")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define REV_IDENT CString("No revision identifier was provided"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"No revision identifier was provided");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"REV_IDENT"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("Preprocessor")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = L"# include \"string\"\n#  pragma __asm \"some library\"\n#define color \"Red\"\nif ShowMessage(\"this is an error\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Red");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"this is an error");
        }

    SECTION("Preprocessor logic block")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#ifdef MYDEFINE \"bogus string\" \\ \nANOTHERDEF \"More bogus text\"\nif ShowMessage(\"this is an error\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("With spaces")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//\"comment\" ASSERT() is a diagnostic function\nif assert  (true && \"this is an error\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        }

    SECTION("Assert")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"/*comment here\nand more commenting about assert(true && \"this is an error\")*/\n\nif assert(true && \"this is an error\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        }
    SECTION("Assert less than token")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"assert(delta >= 0 && \"delta value should be positive when comparing doubles\");";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"delta value should be positive when comparing doubles");
        }

    SECTION("Assert nested tokens")
        {
        cpp_i18n_review cpp;
        // check skipping over "==" to get to NON_UNIT_TEST_ASSERT
        const wchar_t* code = L"NON_UNIT_TEST_ASSERT((end1-begin1) == (end2-begin2) && \"Arrays passed to phi_coefficient must be the same size!\")";
        cpp(code, std::wcslen(code));
        code = L"assert(is_within<double>(pc,-1,1) && \"Error in phi coefficient calculation. Value should be -1 >= and <= 1.\")";
        cpp(code, std::wcslen(code));
        // check skipping over "!=" to get to NON_UNIT_TEST_ASSERT
        code = L"NON_UNIT_TEST_ASSERT((end1-begin1) != (end2-begin2) && \"Arrays passed to phi_coefficient must NOT be the same size!\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 3);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Arrays passed to phi_coefficient must be the same size!");
        CHECK(cpp.get_internal_strings()[1].m_string == L"Error in phi coefficient calculation. Value should be -1 >= and <= 1.");
        CHECK(cpp.get_internal_strings()[2].m_string == L"Arrays passed to phi_coefficient must NOT be the same size!");
        }

    SECTION("Assert Generic")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif CPPUNIT_ASSERT_EQUAL(true && \"this is an error\")";
        cpp(code, std::wcslen(code));
        code = L"//comment\nif CPPUNIT_ASSERT_EQUAL(true && \"this is an error also\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 2);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        CHECK(cpp.get_internal_strings()[1].m_string == L"this is an error also");
        }

    SECTION("Exception")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"throw std::range_error(\"Arrays passed to phi_coefficient must be the same size!\");";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Arrays passed to phi_coefficient must be the same size!");
        }

    SECTION("Buried In Parentheses")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif assert(true && ( (\"this is an error\")))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        }

    SECTION("Localizable In Assert")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif assert(true && _(\"this is an error\"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        REQUIRE(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"this is an error");
        }

    SECTION("String In CTOR")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert(ID, wstring(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String In CTOR variable")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"wstring message( \"Enter your ID.\" );";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"wstring");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"message");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("String In CTOR variable with braces")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"wstring message{ \"Enter your ID.\" };";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"wstring");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"message");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("String In CTOR With Namespace")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert(ID, std::wstring(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Under Scored Word")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"cOnfig_File1", false));
        CHECK(cpp.is_untranslatable_string(L"cOnfig_File_1", false));
        CHECK(cpp.is_untranslatable_string(L"User_cOnfig_File_1", false));
        CHECK(cpp.is_untranslatable_string(L"__HIGH_SCORE__", false));
        }

    SECTION("Encodings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"UTF-8", false));
        CHECK(cpp.is_untranslatable_string(L"utf8", false));
        CHECK(cpp.is_untranslatable_string(L"shift-jis", false));
        CHECK(cpp.is_untranslatable_string(L"shift_jis", false));
        CHECK(cpp.is_untranslatable_string(L"windows-1252", false));
        CHECK(cpp.is_untranslatable_string(L"Big5", false));
        CHECK(cpp.is_untranslatable_string(L"iso-8859-1", false));
        CHECK(cpp.is_untranslatable_string(L"iso-8859-13", false));
        }

    SECTION("Pointless Parens")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"assert(mPipeline.get(), (\"OnPadAdded: unable to allocate stream context\"));";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"OnPadAdded: unable to allocate stream context");
        }

    SECTION("String In CTOR With Template")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert<char>(ID, std::basic_string<char>(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String in CTOR with global namespace")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert<char>(ID, ::basic_string(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        cpp.clear_results();
        // sanity test of empty CTOR
        code = L"if assert<char>(ID, ::(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Comparison in function call")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert(ID==5, \"Bad ID\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Bad ID");
        }

    SECTION("Variable assignment")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::string userMessage = \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::string");
        }

    SECTION("Variable assignment add")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::wstring userMessage += L\"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::wstring");
        }

    SECTION("Variable assignment with pattern")
        {
        cpp_i18n_review cpp;
        cpp.add_variable_name_pattern_to_ignore(std::wregex(L"^test.*"));
        const wchar_t* code = L"std::string testMessage = \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"testMessage");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType == L"std::string");
        }

    SECTION("Variable assignment array")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"char userMessage [3] = \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"char");
        }

    SECTION("Variable assignment template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::basic_string<char> userMessage = \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        // decorations get stripped
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::basic_string");
        }

    SECTION("Variable CTOR")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::string userMessage{ \"Enter your ID.\" }";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::string");
        }

    SECTION("Variable CTOR template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::basic_string<char> userMessage{ \"Enter your ID.\" }";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        // decorations get stripped
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::basic_string");
        }

    SECTION("Variable CTOR complex template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::map<string, int> userMessage{ \"Enter your ID.\", 87 }";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        // decorations get stripped
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::map");
        }

    SECTION("String in parameters with other func calls")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"wxASSERT_MSG(m_defaultReadabilityTestsTemplate.get_test_count() == 0, __WXFUNCTION__ + wxString(\" called twice?\"));";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L" called twice?");
        }
    
    SECTION("Switch statement")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(switch ( GetWindowStyle() & wxBK_ALIGN_MASK )
        {
            case wxBK_TOP:
                [[fallthrough]];
            case wxBK_LEFT:
                // posCtrl is already ok
                break;
            case wxBK_BOTTOM:
                posCtrl.y = sizeClient.y - sizeNew.y;
                break;
            case wxBK_RIGHT:
                posCtrl.x = sizeClient.x - sizeNew.x;
                break;
            default:
                wxFAIL_MSG(L"unexpected alignment");
        })";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"unexpected alignment");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"wxFAIL_MSG");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType.empty());
        }
    
    SECTION("Pointers function")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(handlerInfo = m_frame->GetClassInfo()->
                FindHandlerInfo(wxT("ButtonClickHandler"));)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"ButtonClickHandler");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"FindHandlerInfo");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType.empty());
        CHECK(cpp.get_error_log().empty());
        }

    SECTION("Casts")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(Add(static_cast<CString*>("My ID"));)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"My ID");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"Add");
        }

    SECTION("String escaped")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Skip include")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#include \"string\"\n\nif GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Define variable")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#define REV_TIME \"unknown date and time\"\n\nint i = 9";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"unknown date and time");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"REV_TIME");
        }

    SECTION("Variable initializer list")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(static const char * const effect_play[] = {
            "=	RR",
            "-	RR",
            ";	RR",
            ">	
    RR",
            ",	RR")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 5);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"=	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"-	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[2].m_string == L";	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[2].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[3].m_string == LR"(>	
    RR)");
        CHECK(cpp.get_not_available_for_localization_strings()[3].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[4].m_string == L",	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[4].m_usage.m_value == L"effect_play");
        }

    SECTION("String escaped with escaped slash")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(wxT(\"<img src=\\\"images\\\\\"), \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"<img src=\\\"images\\\\");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Function name pointer")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = (*env)->GetStaticMethodID(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("Function name global namespace")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = ::GetStaticMethodID(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("Function name member")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(what.Printf("standard exception of type \"%s\" with message \"%s\"");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == LR"(standard exception of type \"%s\" with message \"%s\")");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"Printf");
        }

    SECTION("Function name template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = ::GetStaticMethodID<int>(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("String commented paren")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif GetUserInput(/*assert(*/, \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String start escaped")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"blah('\\\"')\nif GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("With Email")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"("An error report has been saved to : \n\"%s\".\n\nPlease email this file to support@company.com to have this issue reviewed. Thank you for your patience.")";
        CHECK_FALSE(cpp.is_untranslatable_string(code, false));
        }

    SECTION("File Filter")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"("Rich Text Format (*.rtf)|*.rtf")";
        CHECK_FALSE(cpp.is_untranslatable_string(code, false));
        }

    SECTION("String in parameters")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(ID, \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Printf commands")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(DateFormat(L"%Y%m%dT%H%M%S");)";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == LR"(%Y%m%dT%H%M%S)");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"DateFormat");
        }

    SECTION("Clipboard")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(MsgBox("\r\nStartHTML:00000000\r\nEndHTML:00000000\r\nStartFragment:00000000\r\nEndFragment:00000000\r\n<html><body>\r\n<!--StartFragment -->\r\n"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        cpp.clear_results();
        code = LR"(MsgBox("Version:0.9\r\nStartHTML:00000000\r\nEndHTML:00000000\r\nStartFragment:00000000\r\nEndFragment:00000000\r\n<html><body>\r\n<!--StartFragment -->\r\n"))";
        cpp(code, std::wcslen(code));
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }
    }
