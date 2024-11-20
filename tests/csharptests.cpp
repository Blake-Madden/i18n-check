#include "../src/csharp_i18n_review.h"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iomanip>
#include <set>
#include <sstream>

// NOLINTBEGIN
using namespace i18n_check;
using namespace Catch::Matchers;

// clang-format off
TEST_CASE("C# code", "[csharp][i18n]")
    {
    SECTION("Raw Strings")
        {
        csharp_i18n_review cs(false);
        const wchar_t* code = LR"(string val = @"This is your last ""chance""!"""; int level;)";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        REQUIRE(cs.get_not_available_for_localization_strings().size() == 1);
        CHECK(cs.get_not_available_for_localization_strings()[0].m_string == std::wstring{ LR"(This is your last "chance"!)" });

        cs.clear_results();

        code = LR"(string cs = @"This is your last chance!"; int level;)";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        REQUIRE(cs.get_not_available_for_localization_strings().size() == 1);
        CHECK(cs.get_not_available_for_localization_strings()[0].m_string == std::wstring{ LR"(This is your last chance!)" });

        cs.clear_results();

        code = LR"(string val = """This is your last ""chance""!"""; int level;)";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        REQUIRE(cs.get_not_available_for_localization_strings().size() == 1);
        CHECK(cs.get_not_available_for_localization_strings()[0].m_string == std::wstring{ LR"(This is your last "chance"!)" });
        }
    }

// NOLINTEND
// clang-format on
