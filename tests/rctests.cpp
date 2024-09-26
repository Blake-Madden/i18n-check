#include "../src/rc_file_review.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTBEGIN
using namespace i18n_check;
using namespace Catch::Matchers;

// clang-format off
TEST_CASE("String tables", "[rc][i18n]")
	{
	SECTION("ID assignments")
		{
		rc_file_review rc;
		const wchar_t* code = LR"(STRINGTABLE
{
    IDS_HELLO,   "\"Hello\""
    IDS_GOODBYE, "Goodbye"
    IDS_SUSPECT, "image.bmp"
} 


STRINGTABLE
BEGIN
IDS_1a "5\xBC-Inch Floppy Disk"
IDS_2 L"Don't confuse \x2229 (intersection) with \x222A (union)."
IDS_3 "Copyright \xA92001"
IDS_3a L"Copyright \x00a92001"
IDS_4 L"%s"
END)";
		rc(code, L"");
		REQUIRE(rc.get_localizable_strings().size() == 6);
		REQUIRE(rc.get_unsafe_localizable_strings().size() == 2);
		CHECK(rc.get_unsafe_localizable_strings()[0].m_string == L"image.bmp");
		CHECK(rc.get_unsafe_localizable_strings()[1].m_string == L"%s");
		}
	}

TEST_CASE("Fonts", "[rc][i18n]")
{
	SECTION("Check system font")
	{
		rc_file_review rc;
		const wchar_t* code = LR"(IDD_ABOUTBOX DIALOGEX 0, 0, 170, 184
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "About..."
FONT 7, "MS Shell Dlg", 0, 0, 0x1
BEGIN

IDD_NewFolderDlg DIALOGEX 0, 0, 321, 50
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Create new folder..."
FONT 18, "MS Shell Dlg", 0, 0, 0x0
BEGIN

IDD_ViewRichFileDlg DIALOGEX 0, 0, 500, 500
STYLE DS_SETFONT | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
CAPTION "Dialog"
FONT 8, "Comic Sans", 0, 0, 0x0
BEGIN)";
		rc(code, L"");
		REQUIRE(rc.get_bad_dialog_font_sizes().size() == 2);
		CHECK(rc.get_bad_dialog_font_sizes()[0].m_string == LR"(7)");
		CHECK(rc.get_bad_dialog_font_sizes()[1].m_string == LR"(18)");

		REQUIRE(rc.get_non_system_dialog_fonts().size() == 1);
		CHECK(rc.get_non_system_dialog_fonts()[0].m_string == LR"(Comic Sans)");
	}
}
// NOLINTEND
// clang-format on
