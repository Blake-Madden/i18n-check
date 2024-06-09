#include "../src/po_file_review.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTBEGIN
using namespace i18n_check;
using namespace Catch::Matchers;

// clang-format off
TEST_CASE("Printf", "[po][l10n]")
	{
	SECTION("Ignore fuzzy and non-formats")
		{
		po_file_review po;
		const wchar_t* code = LR"(#: ../src/common/file.cpp:604
msgid "The server doesn't support the PORT command %s."
msgstr "Сервер не поддерживает команду PORT."

#: ../src/common/file.cpp:612
# fuzzy, c-format
msgid "The server doesn't support %s mode."
msgstr "Сервер не поддерживает пассивный %d.")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 0);
		}

	SECTION("C-format simple")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/gifdecod.cpp:826
#, c-format
msgid "Incorrect frame size (%u, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%u, %d) для frame #%u"

#: ../src/common/gifdecod.cpp:826
#, c-format
msgid "Incorrect frame size (%.5f, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%d, %s) для frame #%u")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 2);
		}

	SECTION("C-format spaces malformed")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/gifdecod.cpp:826
#, c-format
msgid "Incorrect frame size (%u, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%u, % s) для frame #%u")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 1);
		}
	}
// NOLINTEND
// clang-format on
