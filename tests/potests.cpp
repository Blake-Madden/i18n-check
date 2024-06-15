#include "../src/po_file_review.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTBEGIN
using namespace i18n_check;
using namespace Catch::Matchers;

// clang-format off
TEST_CASE("Printf c-format", "[po][l10n]")
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
msgstr "Сервер не поддерживает пассивный %d.

#: ../src/common/file.cpp:604
msgid "The server doesn't support the PORT command %s."
msgstr "Сервер не поддерживает команду PORT."

#: ../src/common/file.cpp:604
msgid "The server doesn't support the PORT command %s."
msgstr "Сервер не поддерживает команду PORT."")";
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
#: ../src/common/decod.cpp:826
#, c-format
msgid "Incorrect frame size (%u, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%u, %d) для frame #%u"
  
#: ../src/common/decod.cpp:826
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
#: ../src/common/decod.cpp:826
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

TEST_CASE("Printf fuzzy", "[po][l10n]")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/decod.cpp:826
#, c-format, fuzzy
msgid "Incorrect frame size (%u, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%u, %d) для frame #%u"
  
#: ../src/common/decod.cpp:826
#, fuzzy, c-format
msgid "Incorrect frame size (%.5f, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%d, %s) для frame #%u")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 0);
		}

TEST_CASE("Printf fuzzy allow", "[po][l10n]")
		{
		po_file_review po;
		po.review_fuzzy_translations(true);
		const wchar_t* code = LR"(
#: ../src/common/decod.cpp:826
#, c-format, fuzzy
msgid "Incorrect frame size (%u, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%u, %d) для frame #%u"
  
#: ../src/common/decod.cpp:826
#, fuzzy, c-format
msgid "Incorrect frame size (%.5f, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%d, %s) для frame #%u")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 2);
		}

TEST_CASE("Printf no format", "[po][l10n]")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/decod.cpp:826
#, no-cpp-format, fuzzy
msgid "Incorrect frame size (%u, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%u, %d) для frame #%u"
  
#: ../src/common/decod.cpp:826
#, no-c-format
msgid "Incorrect frame size (%.5f, %s) for the frame #%u"
msgstr "Неправильный размер кадра (%d, %s) для frame #%u")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 0);
		}

TEST_CASE("Printf c-format percentage", "[po][l10n]")
	{
	po_file_review po;
		const wchar_t* code = LR"(#: ../src/common/file.cpp:604
#, c-format
msgid "Volume %ld%%."
msgstr "Bolumena: %%%ld")";

	po(code, L"");
	po.review_strings();

	const auto issues = std::count_if(
		po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
		{ return ent.second.m_issues.size() > 0; });
	CHECK(issues == 0);
	}

TEST_CASE("Printf c-format percentage has issue", "[po][l10n]")
	{
	po_file_review po;
		const wchar_t* code = LR"(#: ../src/common/file.cpp:604
#, c-format
msgid "Volume %ld%%."
msgstr "Bolumena: %%%d")";

	po(code, L"");
	po.review_strings();

	const auto issues = std::count_if(
		po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
		{ return ent.second.m_issues.size() > 0; });
	CHECK(issues == 1);
	}

TEST_CASE("Printf c-format slash", "[po][l10n]")
	{
	po_file_review po;
		const wchar_t* code = LR"(#: ../src/common/file.cpp:604
#, c-format
msgid "%d/%d blocks at 0x%02x"
msgstr "%d%d bloques en 0x%02x")";

	po(code, L"");
	po.review_strings();

	const auto issues = std::count_if(
		po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
		{ return ent.second.m_issues.size() > 0; });
	CHECK(issues == 0);
	}

TEST_CASE("Printf c-format positionals", "[po][l10n]")
	{
	SECTION("C-format positionals")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/decod.cpp:826
#, c-format
msgid "Incorrect frame size (%u, %s) for the value %.5f"
msgstr "Неправильный размер кадра (%2$s, %1$u) для value %3$.5f")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 0);
		}

	SECTION("C-format positionals multiple usage")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/decod.cpp:826
#, c-format
msgid "Incorrect frame size (%u, %s) for the value %.5f"
msgstr "Неправильный %2$s размер кадра (%2$s, %1$u) для value %3$.5f %1$u")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 0);
		}

	SECTION("C-format positionals mismatching multiple usage")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/decod.cpp:826
#, c-format
msgid "Incorrect frame size (%u, %s) for the value %.5f"
msgstr "Неправильный %2$s размер кадра (%2$d, %1$u) для value %3$.5f %1$u")";
		po(code, L"");
		po.review_strings();

		const auto issues = std::count_if(
			po.get_catalog_entries().cbegin(), po.get_catalog_entries().cend(), [](const auto& ent)
			{ return ent.second.m_issues.size() > 0; });
		CHECK(issues == 1);
		}

	SECTION("C-format positionals mixed with non-positionals")
		{
		po_file_review po;
		const wchar_t* code = LR"(
#: ../src/common/decod.cpp:826
#, c-format
msgid "Incorrect frame size (%u, %s) value"
msgstr "Неправильный %u %2$s")";
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
