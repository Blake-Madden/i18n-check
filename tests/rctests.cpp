#include <iomanip>
#include <sstream>
#include <algorithm>
#include <set>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/rc_file_review.h"

using namespace i18n_check;
using namespace Catch::Matchers;

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
		rc(code);
		REQUIRE(rc.get_localizable_strings().size() == 6);
		REQUIRE(rc.get_unsafe_localizable_strings().size() == 2);
		CHECK(rc.get_unsafe_localizable_strings()[0].m_string == L"image.bmp");
		CHECK(rc.get_unsafe_localizable_strings()[1].m_string == L"%s");
		}
	}
