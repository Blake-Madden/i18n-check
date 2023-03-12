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

    SECTION("HexColor")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"&	c #437A40"));
        }

    SECTION("Xes")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L" xx xx x"));
        }

    SECTION("HTML")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"<tt><span style = 'font-weight:bold;'>"));
        CHECK(cpp.is_untranslatable_string(L"<table style=\\\"width:100%;\"><tr><td width=\"33%\">"));
        CHECK(cpp.is_untranslatable_string(L"<p style=\\\"font-family: %s; font-size: %dpt; color: rgb(%u, %u, %u)\\\">\n"));
        CHECK(cpp.is_untranslatable_string(LR"(<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n<head>\n<meta content=\"text/html; charset=UTF-8\"/>\n<title></title>\n<link href=\"stylesheet.css\" type=\"text/css\" rel=\"stylesheet\" />\n<link rel=\"stylesheet\" type=\"application/vnd.adobe-page-template+xml\" href=\"page-template.xpgt\"/>\n</head>)"));
        }

    SECTION("MacroVariable")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   ("Direct2D failed"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"DX_MSG"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("FuncSig")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(void DefineProperty(const char *name, plcob pb, std::string description="value"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"value"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"description"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("MacroVariableIgnored")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   "Direct2D failed")";
        cpp.add_variable_pattern_to_ignore(std::wregex(L"DX_MSG"));
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == std::wstring(L"DX_MSG"));
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("OrphanString")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(if (value =="my message"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"my message"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L""));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::orphan);
        }

    SECTION("MacroVariableExpandsToFunction")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   MessageBox("Direct2D failed"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"MessageBox"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::function);
        }

    SECTION("AllowPunctuationOnly")
        {
        cpp_i18n_review cpp;
        cpp.allow_translating_punctuation_only_strings(false);
        CHECK(cpp.is_untranslatable_string(L" % "));
        cpp.allow_translating_punctuation_only_strings(true);
        CHECK(!cpp.is_untranslatable_string(L" % "));
        }

    SECTION("WindowsNames")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"Windows 3.1"));
        CHECK(cpp.is_untranslatable_string(L"Windows 98"));
        CHECK(cpp.is_untranslatable_string(L"Windows 2000"));
        CHECK(cpp.is_untranslatable_string(L"Windows NT"));
        CHECK(cpp.is_untranslatable_string(L"Windows NT SP4"));
        CHECK(cpp.is_untranslatable_string(L"Windows XP"));
        CHECK(cpp.is_untranslatable_string(L"Windows XP SP3"));
        CHECK(cpp.is_untranslatable_string(L"Windows Server"));
        CHECK(cpp.is_untranslatable_string(L"Windows Server 2012"));
        CHECK(cpp.is_untranslatable_string(L"Windows Server 2012 R2"));
        CHECK(cpp.is_untranslatable_string(L"Windows Vista"));
        CHECK(cpp.is_untranslatable_string(L"Windows 8"));
        CHECK(cpp.is_untranslatable_string(L"Windows 8.1"));
        CHECK(cpp.is_untranslatable_string(L"Windows 10"));
        }

    SECTION("UnixPath")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"/usr/myfolder/libs"));
        CHECK(cpp.is_untranslatable_string(L"/usr/myfolder/libs/"));
        CHECK(cpp.is_untranslatable_string(L"/usr/myfolder/libs/info.so"));
        CHECK(cpp.is_untranslatable_string(L"/usr/libs/info folder"));
        // not enough slashes to make it appear like a file path
        CHECK(!cpp.is_untranslatable_string(L"/usr is a root folder"));
        }

    SECTION("EscapedQuotes")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"description.Replace(wxT(\"\\\\\\\"\"), wxT(\"/\"));";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 2);
        CHECK(cpp.get_internal_strings()[0].m_string == L"\\\\\\\"");
        CHECK(cpp.get_internal_strings()[1].m_string == L"/");
        cpp.clear_results();
        code = L"view = (mCurTrack[0]->GetWaveformSettings().scaleType == 0) ? wxT(\"\\\"Waveform\\\"\") : wxT(\"\\\"Waveform (dB)\\\"\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"\\\"Waveform\\\"");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"\\\"Waveform (dB)\\\"");
        }

    SECTION("QuoteInSingleQuotes")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if (a == '\"')\nAddCheckBox(_(\"&Use legacy (version 3) syntax.\"),\n(mVersion == 3) ? wxT(\"true\") : wxT(\"false\"));";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 1);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"&Use legacy (version 3) syntax.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"true");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"false");
        }

    SECTION("InternalStrings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L""));
        CHECK(cpp.is_untranslatable_string(L" "));
        CHECK(cpp.is_untranslatable_string(L"  \t"));
        CHECK(cpp.is_untranslatable_string(L"1"));
        CHECK(cpp.is_untranslatable_string(L"1.0"));
        CHECK(cpp.is_untranslatable_string(L">"));
        CHECK(cpp.is_untranslatable_string(L"> "));
        CHECK(cpp.is_untranslatable_string(L"\\n\\t\\r "));
        }

    SECTION("Parens")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"GetMenuBar()->SetLabel(XRCID(\"ID_SAVE_ITEM\"), wxString::Format(_(\"Export %s...\"), GetActiveProjectWindow()->GetName()) );";
        cpp(code, std::wcslen(code));
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
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 3);
        CHECK(cpp.get_internal_strings()[0].m_string == L"xmlHttpRequest");
        CHECK(cpp.get_internal_strings()[1].m_string == L"supportsIpv6OnIos");
        CHECK(cpp.get_internal_strings()[2].m_string == L"xml2HttpRequest");
        }

    SECTION("Encoded")
        {
        cpp_i18n_review cpp;
        // actually Russian text, is OK to translate
        CHECK(!cpp.is_untranslatable_string(L"\\x043f\\x0440\\x0438\\x0432\\x0435\\x0442"));
        }

    SECTION("Internal CSS Strings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"font-style: italic;"));
        CHECK(cpp.is_untranslatable_string(L"  font-style: italic;  "));
        CHECK(cpp.is_untranslatable_string(L"font-weight: bold"));
        CHECK(cpp.is_untranslatable_string(L" color:red"));
        CHECK(cpp.is_untranslatable_string(L"background-COLOR:red"));
        CHECK(cpp.is_untranslatable_string(L" style = 'color: red"));
        }

    SECTION("Internal HTML Strings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"<html>"));
        CHECK(cpp.is_untranslatable_string(L"<!--commment>"));
        CHECK(cpp.is_untranslatable_string(L"<SPAN style="));
        CHECK(!cpp.is_untranslatable_string(L"<HTML>hello"));
        CHECK(cpp.is_untranslatable_string(LR"(<HTML = "hello"></html>)"));
        CHECK(cpp.is_untranslatable_string(LR"(<a href="website"><br>)"));
        CHECK(cpp.is_untranslatable_string(LR"(]]</center>\n)"));
        CHECK(cpp.is_untranslatable_string(L"&amp;"));
        CHECK(cpp.is_untranslatable_string(L"&#107;"));
        CHECK(cpp.is_untranslatable_string(L"&#xF8;"));
        CHECK(cpp.is_untranslatable_string(LR"(charset = \"%s\"\n)"));
        }

    SECTION("Internal XML Strings")
        {
        cpp_i18n_review cpp;
        CHECK(!cpp.is_untranslatable_string(L"<?xml>hello"));
        CHECK(cpp.is_untranslatable_string(L"<?XML>7</XML>"));
        //generic tags
        CHECK(cpp.is_untranslatable_string(L"<doc-val>&entity;</doc-val>"));
        CHECK(cpp.is_untranslatable_string(LR"(<comment =\")"));
        CHECK(cpp.is_untranslatable_string(LR"(<startdoctype name=\"%s\")"));
        CHECK(cpp.is_untranslatable_string(LR"(<image x=%d y=\"%d\" width = '%dpx' height=\"%dpx\")"));
        }

    SECTION("Custom XML")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"<ice>"));
        CHECK(cpp.is_untranslatable_string(L"</ice>"));
        CHECK(cpp.is_untranslatable_string(L"<ice> 9, </ice>"));
        CHECK(cpp.is_untranslatable_string(L"<>"));
        CHECK(cpp.is_untranslatable_string(L"<ice><ice>"));
        CHECK(cpp.is_untranslatable_string(L"<ice-level><ice-level>"));
        CHECK(cpp.is_untranslatable_string(L"<ice> <ice>"));
        CHECK(cpp.is_untranslatable_string(L"<unrecognized version=\"3\">"));
        }

    SECTION("Formulas")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"=color"));
        CHECK(cpp.is_untranslatable_string(L"=small"));
        CHECK(cpp.is_untranslatable_string(L"Open()"));
        }

    SECTION("Internal File Name")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"Log_Rep[o]rt-1.log"));
        CHECK(cpp.is_untranslatable_string(L"c:\\reports\\Log_Report-1.log"));
        CHECK(cpp.is_untranslatable_string(L"c:\\src-stuff,files\\Log_Reporter.h"));
        CHECK(cpp.is_untranslatable_string(L"c:\\users\\yam\\documents&files\\audacity\\mixer\\n\\audacity\\src\\dither.cpp"));
        CHECK(cpp.is_untranslatable_string(L"/src/Log_Reporter.sps9"));
        // double extensions (common on UNIX files)
        CHECK(cpp.is_untranslatable_string(L"dynlib.so.o"));
        // file extension
        CHECK(cpp.is_untranslatable_string(L".sps9"));
        CHECK(cpp.is_untranslatable_string(L"*.sps9"));//wild card
        CHECK(cpp.is_untranslatable_string(L"Log<Report-1.log"));
        // ultra simple relative file path
        CHECK(cpp.is_untranslatable_string(L"shaders/player1.vert"));
        CHECK(cpp.is_untranslatable_string(L"resources\\shaders\\player1.vert"));
        // not really a file name, the ending is deceptively like a file extension
        CHECK(!cpp.is_untranslatable_string(L"The maximum number of notes must be in the range 1..128"));
        }

    SECTION("Variable Name Define")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define SAVE_MAGIC_V1 "fish")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"fish");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"SAVE_MAGIC_V1"));
        }

    SECTION("__Asm")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"__asm int \"A\"\n  __asm {int \"B\"\nint \"C\"}\n\nif b__asm(\"this is an error\")__asm";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("Asm")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"asm int \"A\"\n  asm (int \"B\"\nint \"C\")\n\nif basm(\"this is an error\")asm";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("Asm Volatile")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(asm volatile (                            \
                    "sd  %[val_m],  %[pdst_sd_m]  \n\t"   \
                                                          \
                    : [pdst_sd_m] "=m" (*pdst_sd_m)       \
                    : [val_m] "r" (val_m)                 \
                );if basm("this is an error"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("__Asm__")
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
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("Preprocessor Defined Variable In String Helper")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define REV_IDENT CString("No revision identifier was provided"))";
        cpp(code, std::wcslen(code));
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
        const wchar_t* code = L"# include \"string\"\n#  pragma __asm \"some library\"\n#define color \"Red\"\nif ShowMessage(\"this is an error\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Red");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"this is an error");
        }

    SECTION("Preprocessor Logic Block")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#ifdef MYDEFINE \"bogus string\" \\ \nANOTHERDEF \"More bogus text\"\nif ShowMessage(\"this is an error\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("With Spaces")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//\"comment\" ASSERT() is a diagnostic function\nif assert  (true && \"this is an error\")";
        cpp(code, std::wcslen(code));
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
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        }
    SECTION("Assert Less Than Token")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"assert(delta >= 0 && \"delta value should be positive when comparing doubles\");";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"delta value should be positive when comparing doubles");
        }

    SECTION("Assert Nested Tokens")
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
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String In CTOR With Namespace")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert(ID, std::wstring(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Under Scored Word")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"cOnfig_File1"));
        CHECK(cpp.is_untranslatable_string(L"cOnfig_File_1"));
        CHECK(cpp.is_untranslatable_string(L"User_cOnfig_File_1"));
        CHECK(cpp.is_untranslatable_string(L"__HIGH_SCORE__"));
        }

    SECTION("Encodings")
        {
        cpp_i18n_review cpp;
        CHECK(cpp.is_untranslatable_string(L"UTF-8"));
        CHECK(cpp.is_untranslatable_string(L"utf8"));
        CHECK(cpp.is_untranslatable_string(L"shift-jis"));
        CHECK(cpp.is_untranslatable_string(L"shift_jis"));
        CHECK(cpp.is_untranslatable_string(L"windows-1252"));
        CHECK(cpp.is_untranslatable_string(L"Big5"));
        CHECK(cpp.is_untranslatable_string(L"iso-8859-1"));
        CHECK(cpp.is_untranslatable_string(L"iso-8859-13"));
        }

    SECTION("Pointless Parens")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"assert(mPipeline.get(), (\"OnPadAdded: unable to allocate stream context\"));";
        cpp(code, std::wcslen(code));
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
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String In CTOR With Global Namespace")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert<char>(ID, ::basic_string(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        cpp.clear_results();
        // sanity test of empty CTOR
        code = L"if assert<char>(ID, ::(\"Enter your ID.\"))";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Comparison In Function Call")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert(ID==5, \"Bad ID\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Bad ID");
        }

    SECTION("Variable Assignment")
        {
        cpp_i18n_review cpp;
        cpp.add_variable_pattern_to_ignore(std::wregex(L"^debug"));
        const wchar_t* code = L"std::string userMessage = \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        REQUIRE(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Variable Assignment Add")
        {
        cpp_i18n_review cpp;
        cpp.add_variable_pattern_to_ignore(std::wregex(L"^debug"));
        const wchar_t* code = L"std::wstring userMessage += L\"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        REQUIRE(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Variable Assignment With Pattern")
        {
        cpp_i18n_review cpp;
        cpp.add_variable_pattern_to_ignore(std::wregex(L"^debug.*"));
        const wchar_t* code = L"std::string debugMessage = \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Variable Assignment Array")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"userMessage [3] = \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        REQUIRE(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("StringInParametersWithOtherFuncCalls")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"wxASSERT_MSG(m_defaultReadabilityTestsTemplate.get_test_count() == 0, __WXFUNCTION__ + wxString(\" called twice?\"));";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L" called twice?");
        }

    SECTION("String Escaped")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Skip Include")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#include \"string\"\n\nif GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Define Variable")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#define REV_TIME \"unknown date and time\"\n\nint i = 9";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"unknown date and time");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"REV_TIME");
        }

    SECTION("VariableInitializerList")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(static const char * const effect_play_xpm[] = {
            "=	RR",
            "-	RR",
            ";	RR",
            ">	
    RR",
            ",	RR")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 5);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"=	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"effect_play_xpm");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"-	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_usage.m_value == L"effect_play_xpm");
        CHECK(cpp.get_not_available_for_localization_strings()[2].m_string == L";	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[2].m_usage.m_value == L"effect_play_xpm");
        CHECK(cpp.get_not_available_for_localization_strings()[3].m_string == LR"(>	
    RR)");
        CHECK(cpp.get_not_available_for_localization_strings()[3].m_usage.m_value == L"effect_play_xpm");
        CHECK(cpp.get_not_available_for_localization_strings()[4].m_string == L",	RR");
        CHECK(cpp.get_not_available_for_localization_strings()[4].m_usage.m_value == L"effect_play_xpm");
        }

    SECTION("StringEscapedWithEscapedSlash")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(wxT(\"<img src=\\\"images\\\\\"), \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"<img src=\\\"images\\\\");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Function Name Pointer")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = (*env)->GetStaticMethodID(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("Function Name Global Namespace")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = ::GetStaticMethodID(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("Function Name Member")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(what.Printf("standard exception of type \"%s\" with message \"%s\"");)";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == LR"(standard exception of type \"%s\" with message \"%s\")");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"Printf");
        }

    SECTION("Function Name Template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = ::GetStaticMethodID<int>(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("String Commented Paren")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif GetUserInput(/*assert(*/, \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String Start Escaped")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"blah('\\\"')\nif GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("With Email")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"("An error report has been saved to : \n\"%s\".\n\nPlease email this file to support@company.com to have this issue reviewed. Thank you for your patience.")";
        CHECK(!cpp.is_untranslatable_string(code));
        }

    SECTION("File Filter")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"("Rich Text Format (*.rtf)|*.rtf")";
        CHECK(!cpp.is_untranslatable_string(code));
        }

    SECTION("String In Parameters")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(ID, \"Enter your ID.\")";
        cpp(code, std::wcslen(code));
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }
    }