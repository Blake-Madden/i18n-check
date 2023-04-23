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
        remove_escaped_unicode_values(str);
        CHECK(str == L"");
        }

    SECTION("Empty all removed")
        {
        std::wstring str = LR"(\u266f\u266d)"; // ♯♭
        remove_escaped_unicode_values(str);
        CHECK(str == L"            ");
        }

    SECTION("Boundary Check")
        {
        std::wstring str = LR"(\u266f\u266d\26)"; // ♯♭\26
        remove_escaped_unicode_values(str);
        CHECK(str == LR"(            \26)");
        }

    SECTION("X Digits")
        {
        std::wstring str = L"Russian: \\x043f\\x0440\\x0438\\x0432\\x0435\\x0442 and Pound Sterling: \\xA3.";
        remove_escaped_unicode_values(str);
        // привет and £
        CHECK(str == L"Russian:                                        and Pound Sterling:     .");
        }

    SECTION("Digits")
        {
        std::wstring str = L"Russian: \\u043f\\u0440\\u0438\\u0432\\u0435\\u0442";
        remove_escaped_unicode_values(str);
        // привет
        CHECK(str == L"Russian:                                     ");
        }

    SECTION("32-Bit Digits")
        {
        std::wstring str = L"Some text \\U000FF254.";
        remove_escaped_unicode_values(str);
        CHECK(str == L"Some text         .");
        }

    SECTION("Bogus Specifiers")
        {
        std::wstring str = L"F\\u2H6f/G\\u266";
        remove_escaped_unicode_values(str);
        CHECK(str == L"F\\u2H6f/G\\u266");
        }
    }