#include "../src/cpp_i18n_review.h"
#include "../src/i18n_review.h"
#include "../src/i18n_string_util.h"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iomanip>
#include <set>
#include <sstream>

// NOLINTBEGIN
using namespace i18n_string_util;
using namespace i18n_check;
using namespace Catch::Matchers;

// clang-format off
TEST_CASE("untranslatable", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    CHECK(reviewer.is_untranslatable_string(L"application/x-photoshop-style-library", false));
    CHECK(reviewer.is_untranslatable_string(L"image/webp", false));
    CHECK(reviewer.is_untranslatable_string(L"ms-appdata", false));
    CHECK(reviewer.is_untranslatable_string(L"&perms=write&frob=", false));
    CHECK(reviewer.is_untranslatable_string(L"Content-Type: multipart/form-data; boundary=EBA799EB-D9A2-472B-AE86-568D4645707E\r\n", false));
    CHECK(reviewer.is_untranslatable_string(L"Content-Disposition: form-data; name=\"photo\"; filename=\"", false));
    CHECK_FALSE(reviewer.is_untranslatable_string(L"<No Name Specified>", false));
    // percentages can be translatable as the ordering of the number and % can be changed
    CHECK_FALSE(reviewer.is_untranslatable_string(L"100%", false));
    CHECK_FALSE(reviewer.is_untranslatable_string(L"{n}%", false));
    CHECK_FALSE(reviewer.is_untranslatable_string(L"%d%", false));
    // HTML anchors
    CHECK(reviewer.is_untranslatable_string(L"#something", false));
    CHECK(reviewer.is_untranslatable_string(L"#Something-else", false));
    CHECK(reviewer.is_untranslatable_string(L"#something-else", false));
    CHECK(reviewer.is_untranslatable_string(L"#somethingElse", false));
    }

TEST_CASE("i18n string utils", "[i18nstringutil]")
    {
    SECTION("Conversions to 7-bit")
        {
        CHECK(full_width_number_to_7bit(L'０') == L'0');
        CHECK(full_width_number_to_7bit(L'９') == L'9');
        CHECK(full_width_number_to_7bit(L'9') == L'9');
        CHECK(full_width_number_to_7bit(L'w') == L'w');

        CHECK(devanagari_number_to_7bit(L'०') == L'0');
        CHECK(devanagari_number_to_7bit(L'९') == L'9');

        // wrong converter will return unaltered value
        CHECK(full_width_number_to_7bit(L'९') == L'९');

        // pass through multiple converters
        CHECK(devanagari_number_to_7bit(full_width_number_to_7bit(L'९')) == L'9');

        // integration
        const auto numbers = i18n_review::load_numbers(L"८८५९-१");
        REQUIRE(numbers.size() == 2);
        CHECK(numbers[0] == std::wstring{ L"1" });
        CHECK(numbers[1] == std::wstring{ L"8859" });
        }

    SECTION("Conversions from 7-bit")
        {
        CHECK(seven_bit_number_to_full_width(L'0') == L'０');
        CHECK(seven_bit_number_to_full_width(L'9') == L'９');
        CHECK(seven_bit_number_to_full_width(L'９') == L'９');
        CHECK(seven_bit_number_to_full_width(L'w') == L'w');

        CHECK(seven_bit_number_to_devanagari(L'0') == L'०');
        CHECK(seven_bit_number_to_devanagari(L'9') == L'९');

        // wrong converter will return unaltered value
        CHECK(seven_bit_number_to_full_width(L'९') == L'९');
        }
    }

TEST_CASE("File Paths", "[i18nstringutil]")
    {
    SECTION("Null")
        {
        CHECK_FALSE(is_file_address(L""));
        }
    SECTION("Typo Not Really A File Path")
        {
        CHECK_FALSE(is_file_address(L"file.Rob"));
        CHECK(is_file_address(L"file.TIF"));
        CHECK(is_file_address(L"file.tif"));
        // should be the start of a new sentence with the word "Tiff"
        CHECK_FALSE(is_file_address(L"file.Tif"));
        }
    SECTION("Non Addresses")
        {
        CHECK_FALSE(is_file_address(L"text"));
        CHECK_FALSE(is_file_address(L"FTPs"));
        CHECK_FALSE(is_file_address(L"wwwhat!"));
        CHECK_FALSE(is_file_address(L"File Names: (.cpp/.h)"));
        CHECK_FALSE(is_file_address(L".cpp/.h)"));
        }
    SECTION("Domains")
        {
        CHECK(is_file_address(L"http://www.sales.mycompany.com") );
        CHECK(is_file_address(L"HTTPS://www.sales.mycompany.com") );
        CHECK(is_file_address(L"ftp://www.sales.mycompany.com") );
        CHECK(is_file_address(L"www.sales.mycompany.com") );
        }
    SECTION("Domains No Prefix")
        {
        CHECK(is_file_address(L"ibm.com/") );
        CHECK(is_file_address(L"ibm.com/software/data/infosphere/identity-insight") );
        CHECK_FALSE(is_file_address(L"123.578/72")); // avoid what is really an equation
        CHECK(is_file_address(L"go.businessmachines.com"));
        CHECK(is_file_address(L"go.businessmachines.com/"));
        }
    SECTION("Network Paths")
        {
        CHECK(is_file_address(L"\\\\server") );
        CHECK(is_file_address(L"file://blah") );
        }
    SECTION("Windows Paths")
        {
        CHECK(is_file_address(L"C:\\users") );
        CHECK(is_file_address(L"C:\\") );
        CHECK(is_file_address(L"C:/users") ); // wrong path separator should still work
        CHECK(is_file_address(L"D:\\users") );
        CHECK_FALSE(is_file_address(L"7:\\users")); // not a real drive letter
        CHECK_FALSE(is_file_address(L"C:")); // not long enough
        }
    SECTION("UNIX Paths")
        {
        CHECK(is_file_address(L"/Users/Share") );
        CHECK_FALSE(is_file_address(L"/Users")); // not enough folders to determine if a real path
        CHECK_FALSE(is_file_address(L"/a")); // not long enough
        }
    SECTION("Email Paths")
        {
        CHECK(is_file_address(L"mailto:me") );
        CHECK(is_file_address(L"blake@mail.com") );
        CHECK_FALSE(is_file_address({ L"blake@mail.com", 10 }) ); // doesn't read far enough to get to the dot
        CHECK_FALSE(is_file_address({ L"blake@mail.com", 11 }) ); // doesn't read far enough to get to the domain
        CHECK(is_file_address({ L"blake@mail.com", 12 })); // "c" will be the domain
        CHECK_FALSE(is_file_address({ L"blake@mail", 10 })); // missing domain
        CHECK_FALSE(is_file_address({ L"bl.ke@mail", 10 })); // missing domain
        }
    SECTION("File Names")
        {
        CHECK(is_file_address(L"file.bmp") );
        CHECK_FALSE(is_file_address(L"file.bm"));
        CHECK_FALSE(is_file_address(L"file.bmps"));
        CHECK_FALSE(is_file_address(L"file.787"));
        CHECK(is_file_address(L"file.h"));
        CHECK(is_file_address(L"file.c"));
        CHECK(is_file_address(L"stdafx.h"));
        CHECK_FALSE(is_file_address(L".h"));
        CHECK(is_file_address(L"a.docx"));
        CHECK(is_file_address(L"libreoffice.tar.xz"));
        }

    SECTION("Short Url")
        {
        CHECK(i18n_string_util::is_url(L"amazon.com"));
        CHECK(i18n_string_util::is_url(L"shop.amazon.com"));
        CHECK(i18n_string_util::is_url(L"amazon.com's"));
        CHECK(i18n_string_util::is_url(L"amazon.com’s"));
        CHECK(!i18n_string_util::is_url({ L"amazon.txt’s", 12 }));

        CHECK(i18n_string_util::is_url(L"amazon.au"));
        CHECK(!i18n_string_util::is_url(L"amazon."));
        CHECK(!i18n_string_util::is_url(L"amazon.a"));
        }

     SECTION("Bogus Url/File")
        {
        CHECK_FALSE(i18n_string_util::is_url(L"Return to this window after you have finished the authorization process on Flickr.com"));
        CHECK_FALSE(i18n_string_util::is_file_address(L"Insert .tga"));
        CHECK_FALSE(i18n_string_util::is_file_address(L"Insert .baml"));
        CHECK_FALSE(i18n_string_util::is_file_address(L"Insert .html"));
        }

    SECTION("Short File")
        {
        CHECK(i18n_string_util::is_file_address(L"stdafx.h"));
        CHECK(i18n_string_util::is_file_address(L"stdafx.h's"));
        CHECK(i18n_string_util::is_file_address(L"stdafx.h’s"));
        }
    }

TEST_CASE("Printf remove", "printf")
    {
    SECTION("Empty")
        {
        std::wstring str = L"";
        remove_printf_commands(str);
        CHECK(str == L"");
        }

    SECTION("Numbers")
        {
        std::wstring str = L"Number %d and another %lu, and another %-5.06f and another %#x.";
        remove_printf_commands(str);
        CHECK(str == L"Number  and another , and another  and another .");
        }

    SECTION("Espcaped")
        {
        std::wstring str = L"A percent 5%% and another \\% one.";
        remove_printf_commands(str);
        CHECK(str == L"A percent 5%% and another \\% one.");
        }

    SECTION("Simple")
        {
        std::wstring str = L"This is simple %s, this is too %c.";
        remove_printf_commands(str);
        CHECK(str == L"This is simple , this is too .");
        }

    SECTION("Boundary Check")
        {
        std::wstring str = L"%s%i%c";
        remove_printf_commands(str);
        CHECK(str == L"");
        }

    SECTION("Bogus Specifiers")
        {
        std::wstring str = L"This isn't correct %y, neither is this %.5q. This is out of order %5#f.";
        remove_printf_commands(str);
        CHECK(str == L"This isn't correct %y, neither is this %.5q. This is out of order %5#f.");
        }
    }

TEST_CASE("Hex color remove", "[hexcolor]")
    {
    SECTION("Empty")
        {
        std::wstring str = L"";
        remove_hex_color_values(str);
        CHECK(str == L"");
        }

    SECTION("Boundary Check")
        {
        std::wstring str = L"#FF01Ab#ab993D";
        remove_hex_color_values(str);
        CHECK(str == L"");
        }

    SECTION("Colors")
        {
        std::wstring str = L"My color is #FF01Ab and #ab993D.";
        remove_hex_color_values(str);
        CHECK(str == L"My color is  and .");
        }

    SECTION("Bogus Specifiers")
        {
        std::wstring str = L"My color is #ZF01Ab and #ab993.";
        remove_hex_color_values(str);
        CHECK(str == L"My color is #ZF01Ab and #ab993.");
        }
    }

// NOLINTEND
// clang-format on
