#include <iomanip>
#include <sstream>
#include <algorithm>
#include <set>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/i18n_string_util.h"

using namespace i18n_string_util;
using namespace Catch::Matchers;

TEST_CASE("i18n string util", "[i18nstringutil]")
    {
    SECTION("Null")
        {
        CHECK_FALSE(is_file_address(nullptr, 1));
        CHECK_FALSE(is_file_address(L"text", 0));
        }
    SECTION("Typo Not Really A File Path")
        {
        CHECK_FALSE(is_file_address(L"file.Rob", 8));
        CHECK(is_file_address(L"file.TIF", 8));
        CHECK(is_file_address(L"file.tif", 8));
        // should be the start of a new sentence with the word "Tiff"
        CHECK_FALSE(is_file_address(L"file.Tif", 8));
        }
    SECTION("Non Addresses")
        {
        CHECK_FALSE(is_file_address(L"text", 4));
        CHECK_FALSE(is_file_address(L"FTPs", 4));
        CHECK_FALSE(is_file_address(L"wwwhat!", 7));
        }
    SECTION("Domains")
        {
        CHECK(is_file_address(L"http://www.sales.mycompany.com", 30) );
        CHECK(is_file_address(L"HTTPS://www.sales.mycompany.com", 31) );
        CHECK(is_file_address(L"ftp://www.sales.mycompany.com", 29) );
        CHECK(is_file_address(L"www.sales.mycompany.com", 23) );
        }
    SECTION("Domains No Prefix")
        {
        CHECK(is_file_address(L"ibm.com/", 8) );
        CHECK(is_file_address(L"ibm.com/software/data/infosphere/identity-insight", 49) );
        CHECK_FALSE(is_file_address(L"123.578/72", 10));//avoid what is really an equation
        CHECK(is_file_address(L"go.businessmachines.com", 23));
        CHECK(is_file_address(L"go.businessmachines.com/", 24));
        }
    SECTION("Network Paths")
        {
        CHECK(is_file_address(L"\\\\server", 8) );
        CHECK(is_file_address(L"file://blah", 11) );
        }
    SECTION("Windows Paths")
        {
        CHECK(is_file_address(L"C:\\users", 8) );
        CHECK(is_file_address(L"C:/users", 8) );//wrong path separator should still work
        CHECK(is_file_address(L"D:\\users", 8) );
        CHECK_FALSE(is_file_address(L"7:\\users", 8));//not a real drive letter
        CHECK_FALSE(is_file_address(L"C:", 2));//not long enough
        }
    SECTION("UNIX Paths")
        {
        CHECK(is_file_address(L"/Users/Share", 12) );
        CHECK_FALSE(is_file_address(L"/Users", 6));//not enough folders to determine if a real path
        CHECK_FALSE(is_file_address(L"/a", 2));//not long enough
        }
    SECTION("Email Paths")
        {
        CHECK(is_file_address(L"mailto:me", 9) );
        CHECK(is_file_address(L"blake@mail.com", 14) );
        CHECK_FALSE(is_file_address(L"blake@mail.com", 10) );//doesn't read far enough to get to the dot
        CHECK_FALSE(is_file_address(L"blake@mail.com", 11) );//doesn't read far enough to get to the domain
        CHECK(is_file_address(L"blake@mail.com", 12));//"c" will be the domain
        CHECK_FALSE(is_file_address(L"blake@mail", 10));//missing domain
        CHECK_FALSE(is_file_address(L"bl.ke@mail", 10));//missing domain
        }
    SECTION("File Names")
        {
        CHECK(is_file_address(L"file.bmp", 8) );
        CHECK_FALSE(is_file_address(L"file.bm", 7));
        CHECK_FALSE(is_file_address(L"file.bmps", 9));
        CHECK_FALSE(is_file_address(L"file.787", 8));
        CHECK(is_file_address(L"file.h", 6));
        CHECK(is_file_address(L"file.c", 6));
        CHECK(is_file_address(L"stdafx.h", 8));
        CHECK_FALSE(is_file_address(L".h", 2));
        CHECK(is_file_address(L"a.docx", 6));
        CHECK(is_file_address(L"libreoffice.tar.xz", 18));
        }

    SECTION("Is Short Url")
        {
        CHECK(i18n_string_util::is_url(L"amazon.com", 10));
        CHECK(i18n_string_util::is_url(L"shop.amazon.com", 15));
        CHECK(i18n_string_util::is_url(L"amazon.com's", 12));
        CHECK(i18n_string_util::is_url(L"amazon.com’s", 12));
        CHECK(!i18n_string_util::is_url(L"amazon.txt’s", 12));

        CHECK(i18n_string_util::is_url(L"amazon.au", 9));
        CHECK(!i18n_string_util::is_url(L"amazon.", 7));
        CHECK(!i18n_string_util::is_url(L"amazon.a", 8));
        }

    SECTION("Is Short File")
        {
        CHECK(i18n_string_util::is_file_address(L"stdafx.h", 8));
        CHECK(i18n_string_util::is_file_address(L"stdafx.h's", 10));
        CHECK(i18n_string_util::is_file_address(L"stdafx.h’s", 10));
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