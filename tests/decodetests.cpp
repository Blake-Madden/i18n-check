#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/i18n_string_util.h"

using namespace Catch::Matchers;
using namespace i18n_string_util;

TEST_CASE("Text decode", "[decode]")
    {
    SECTION("Empty")
        {
        std::wstring str = L"";
        decode_escaped_unicode_values(str);
        CHECK(str == L"");
        }

    SECTION("Boundary Check")
        {
        std::wstring str = L"\\u266f\\u266d";
        decode_escaped_unicode_values(str);
        CHECK(str == L"♯♭");
        }

    SECTION("X Digits")
        {
        std::wstring str = L"Russian: \\x043f\\x0440\\x0438\\x0432\\x0435\\x0442 and Pound Sterling: \\xA3.";
        decode_escaped_unicode_values(str);
        CHECK(str == L"Russian: привет and Pound Sterling: £.");
        }

    SECTION("Digits")
        {
        std::wstring str = L"Russian: \\u043f\\u0440\\u0438\\u0432\\u0435\\u0442";
        decode_escaped_unicode_values(str);
        CHECK(str == L"Russian: привет");
        }

    SECTION("32-Bit Digits")
        {
        std::wstring str = L"Some text \\U000FF254.";
        // just have to be lost, can't support 32-bit unicode
        decode_escaped_unicode_values(str);
        CHECK(str == L"Some text .");
        }

    SECTION("Bogus Specifiers")
        {
        std::wstring str = L"F\\u2H6f/G\\u266";
        decode_escaped_unicode_values(str);
        CHECK(str == L"F\\u2H6f/G\\u266");
        }
    }