#include "../src/cpp_i18n_review.h"
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
TEST_CASE("Snake case words", "[cpp][i18n]")
    {
    SECTION("user_level_permission")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "user_level_permission";)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_internal_strings().size() == 1);
        }
    SECTION("__HIGH_SCORE__")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "__HIGH_SCORE__";)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_internal_strings().size() == 1);
        }
    SECTION("__HIGH_SCORE__")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "Config_File_Path";)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_internal_strings().size() == 1);
        }
    SECTION("P_rinter")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "P_rinter";)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        }
    }

TEST_CASE("<<", "[cpp][i18n]")
    {
    SECTION("<< with params")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(qCDebug(KDE_LOG) << "Rendered image")";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_internal_strings().size() == 1);
        }

    SECTION("<< empty params")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(qDebug() << "################### THERE IS A MESSAGE #################";)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_internal_strings().size() == 1);
        }

    SECTION("Doesn't support <<")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxGetTranslation("Hello there") << "################### THERE IS A MESSAGE #################";)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"Hello there");
        }
    }

TEST_CASE("Place holders", "[cpp][i18n]")
    {
    SECTION("Xes")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = _(L"XXXXXX XXXXXX");)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        }

    SECTION("X numbers")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = _(L"XXXXXX -X.XXXXX, +X.XXXXX");)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        }
    }

TEST_CASE("Version Info", "[cpp][i18n]")
    {
    SECTION("vString")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = _(L"ClientTest v1.2");)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        }
    }

TEST_CASE("C/C++ code", "[cpp][i18n]")
    {
    SECTION("Pointer")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = _(L"pVal->Dosomething();");)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        }
    }

TEST_CASE("IDs", "[cpp][i18n]")
    {
    SECTION("ID assignments")
        {
        cpp_i18n_review cpp;
        cpp.set_style(review_style::all_i18n_checks);
        const wchar_t* code = LR"(int val = 5;
static int const MENU_ID_NEW = 1000;
wxWindowID MENU_ID_SAVE = -1001;
uint32_t MENU_ID_PRINT{ 1'002 };
UINT ID_EXPORT{ 1003 };
UINT ID_EXPORT_AS{ wxID_HIGHEST };
UINT ID_PRINT_ALL(wxID_HIGHEST+1);)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_ids_assigned_number().size() == 4);
        CHECK(cpp.get_ids_assigned_number()[0].m_string == L"1000 assigned to MENU_ID_NEW");
        CHECK(cpp.get_ids_assigned_number()[1].m_string == L"-1001 assigned to MENU_ID_SAVE");
        CHECK(cpp.get_ids_assigned_number()[2].m_string == L"1002 assigned to MENU_ID_PRINT");
        CHECK(cpp.get_ids_assigned_number()[3].m_string == L"1003 assigned to ID_EXPORT");
        }

    SECTION("ID assignment define")
        {
        cpp_i18n_review cpp;
        cpp.set_style(review_style::all_i18n_checks);
        const wchar_t* code = LR"(#define IDD_ABOUTBOX                    100
#define IDP_OLE_INIT_FAILED             101
#define IDD_ADDTEXT                     103
#define IDR_MAINFRAME                   104
#define IDS_WINDOWS_MANAGER             107
#define IDD_DIALOG1                     110
#define IDD_EMBED                       111
#define IDC_CURSOR_MS                   112
#define IDC_CURSOR1                     113
#define IDD_ZOOM_TO_COORDS              114
#define IDC_CURSOR2                     115
#define IDR_CSM_VIEW                    116
#define IDC_CURSOR3                     117
#define IDC_CURSOR4                     119
#define IDD_ANIMATE                     121
#define IDD_EDIT_COLORS                 122
#define IDB_COLORS                      123
#define IDB_PICTURE                     124
#define IDB_DONATE                      126
#define IDD_HELPDIALOG                  127
#define IDC_CURSOR5                     330
#define IDD_DIALOG2                     331
#define IDD_ADDPICTURE                  331
#define IDB_TT                          332
#define IDB_TTR                         333
#define IDC_EDIT_WIDTH                  1000
#define IDC_EDIT_HEIGHT                 1001
#define IDC_EDIT_XMIN                   1002
#define IDC_EDIT_XMAX                   1003
#define IDC_EDIT_YMIN                   1004
#define IDC_EDIT_YMAX                   1005
#define IDC_COMBO_RANGE                 1006
#define IDC_EDIT_ITER                   1007
#define IDC_EDIT_P                      1008
#define IDC_FONT                        1009
#define IDC_CHECK_SMOOTH                1010
#define IDC_CHECK_RED                   1011
#define IDC_CHECK_GREEN                 1012
#define IDC_CHECK_BLUE                  1013
#define IDC_EMBEDDED                    1015
#define IDC_RANDOM                      1018
#define IDC_EDIT_RG                     1019
#define IDC_BASE                        1020
#define IDC_EDIT_RB                     1021
#define IDC_RADIO_EXISTING              1022
#define IDC_OPACITY                     1025
#define IDC_TEXT_OPACITY                1026
#define IDC_RADIO_NEW                   1027
#define IDC_BOLD                        1028
#define IDC_EDIT_ITER3                  1029
#define IDC_EDIT_CALC                   1030
#define IDC_STATIC_COORD                1031
#define IDC_ITALICS                     1032
#define IDC_CHECK_SMOOTH2               1033
#define IDC_CHECK_DROPUNUSED            1034
#define IDC_CHECK_GRADIENT              1035
#define IDC_AUTOPOINT                   1036
#define IDC_ITALICS2                    1037
#define IDC_NOBACKGROUND                1038
#define IDC_GRADIENT1                   1039
#define IDC_POINTSIZE                   1040
#define IDC_GRADIENT2                   1041
#define IDC_CHECK_BASE                  1042
#define IDC_BACKCOLOR                   1043
#define IDC_GRADIENT_COLORS             1044
#define IDC_SORT_LESS                   1045
#define IDC_SORT_GREATER                1046
#define IDC_RANDOM_REVERSE              1047
#define IDC_REVERSE                     1048
#define IDC_CHOOSE_IMAGE                1049
#define IDC_CHECK_USEIMAGE              1050
#define IDC_STEPCOLOR                   1051
#define IDC_CHECK_BLENDBASE             1053
#define IDC_CHOOSE_IMAGE2               1056
#define IDC_STATIC_YMAX                 1057
#define IDC_STATIC_YMIN                 1058
#define IDC_STATIC_XMIN                 1059
#define IDC_STATIC_XMAX                 1060
#define IDC_RADIO_BASE_COLOR            1070
#define IDC_EDIT1                       1071
#define IDC_EDIT_PICTURENAME            1071
#define IDC_RADIO_BASE_AD               1072
#define IDC_CHOOSE_PICTURE              1072
#define IDC_EDIT_OPACITY                1080
#define IDC_STATIC_OPACITY              1082
#define IDC_EDIT_TEXT                   1084
#define IDC_GRADIENT5                   1088
#define IDC_TEXTCOLOR                   1089
#define IDC_BOLD_NORMAL                 1091
#define IDC_RADIO_PATH_COLOR_BASE       1092
#define IDC_RESET                       1094
#define IDC_BOLD_HEAVY                  1096
#define IDC_EDIT_COLORS                 1098
#define IDC_STATIC_FONT                 1099
#define IDC_EDIT_MODE                   32784
#define IDC_STATIC_POINTSIZE            1103
#define IDC_CHECK1                      1110
#define IDC_CHECK_TRANSPARENT_COLOR     1110
#define IDC_CHECK_FLYINTO               1111
#define IDC_CHECK_FLYFRAMES             1112
#define IDC_DONATE                      1113
#define IDC_EDIT_FLYFRAMES              1114
#define IDC_HELPDIALOG                  1115
#define IDC_STATIC_BASECOLOR            1118
#define IDC_CHECK_PIXELS                1120
#define IDC_STATIC_INFO                 1122
#define IDC_STATIC_STEP                 1124
#define IDC_LIST_COLORS                 1125
#define IDC_TRANSPARENT_COLOR           1134
#define IDC_BOLDSLIDER                  1135
#define IDC_STATIC_WEIGHT               1136);)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_ids_assigned_number().size() == 1);
        CHECK(cpp.get_ids_assigned_number()[0].m_string == L"32784 assigned to IDC_EDIT_MODE; value should be between 1 and 0x6FFF if this is an MFC project.");
        }

    SECTION("Duplicate ID value assignments")
        {
        cpp_i18n_review cpp;
        cpp.set_style(review_style::all_i18n_checks);
        const wchar_t* code = LR"(int val = 5;
static int const MENU_ID_NEW = 1000;
wxWindowID MENU_ID_SAVE = wxID_HIGHEST;
uint32_t MENU_ID_PRINT{ 1'002 };
UINT ID_EXPORT{ wxID_HIGHEST + 1 };
UINT ID_EXPORT_AS{ wxID_HIGHEST+1 };
UINT ID_PRINT_ALL(wxID_HIGHEST  +1);)";
        cpp(code, L"");
        cpp.review_strings();
        // complain about both duplicate assignments
        REQUIRE(cpp.get_duplicates_value_assigned_to_ids().size() == 2);
        CHECK(cpp.get_duplicates_value_assigned_to_ids()[0].m_string == L"wxID_HIGHEST+1 has been assigned to multiple ID variables.");
        CHECK(cpp.get_duplicates_value_assigned_to_ids()[0].m_string == L"wxID_HIGHEST+1 has been assigned to multiple ID variables.");
        }

    SECTION("Duplicate ID value assignments same variable")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(int val = 5;
static int const MENU_ID_NEW = 1000;
wxWindowID MENU_ID_NEW = 1000;
wxWindowID MENU_ID_SAVE = wxID_HIGHEST;)";
        cpp(code, L"");
        cpp.review_strings();
        // same variable name and value assigned, so ignore that (may be intentional assignments in diffence code sections)
        CHECK(cpp.get_duplicates_value_assigned_to_ids().size() == 0);
        }

    SECTION("ID assignments in parentheses")
        {
        cpp_i18n_review cpp;
        cpp.set_style(review_style::all_i18n_checks);
        const wchar_t* code = LR"(int val = 5;
static int const MENU_ID_NEW = (({ 1000}));
wxWindowID MENU_ID_SAVE =     (((1001)));
uint32_t MENU_ID_PRINT{ (1'002) };
UINT ID_EXPORT{ (({1000})) };)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_ids_assigned_number().size() == 4);
        CHECK(cpp.get_ids_assigned_number()[0].m_string == L"1000 assigned to MENU_ID_NEW");
        CHECK(cpp.get_ids_assigned_number()[1].m_string == L"1001 assigned to MENU_ID_SAVE");
        CHECK(cpp.get_ids_assigned_number()[2].m_string == L"1002 assigned to MENU_ID_PRINT");
        CHECK(cpp.get_ids_assigned_number()[3].m_string == L"1000 assigned to ID_EXPORT");
        REQUIRE(cpp.get_duplicates_value_assigned_to_ids().size() == 1);
        CHECK(cpp.get_duplicates_value_assigned_to_ids()[0].m_string == L"1000 has been assigned to multiple ID variables.");
        }

    SECTION("ID value assignments from function")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(const wxWindowID listID = lua_tonumber(L, 2);
const wxWindowID reportID = lua_tonumber(L, 3);)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_duplicates_value_assigned_to_ids().size() == 0);
        }
    
    SECTION("ID value assignments not ID var")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(static const int THUMBNAIL_WIDTH = 320;
    static const int THUMBNAIL_HEIGHT = 240;)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_ids_assigned_number().size() == 0);
        }
    
    SECTION("ID value assignments from member function")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(int baseYearID = Model_Budgetyear::instance().Get(baseYear);
            int newYearID  = Model_Budgetyear::instance().Get(currYearText);)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_duplicates_value_assigned_to_ids().size() == 0);
        }

    SECTION("ID value assignments from object")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(int fromAccountID = from_account->ACCOUNTID;
            int fromAccountID = from_account->USER_ACCOUNTID;)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_duplicates_value_assigned_to_ids().size() == 0);
        }
    
    SECTION("ID value assignments not ID var")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(static const int THUMBNAIL_WIDTH = 320;
    static const int THUMBNAIL_HEIGHT = 240;)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_ids_assigned_number().size() == 0);
        }

    SECTION("ID value assignments to ignore")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(static int const MENU_ID_NEW = wxID_ANY;
static int const MENU_ID_PRINT = -1;
static int const MENU_ID_PRINT_ALL = -1;
static int const MENU_ID_EXPORT = wxID_ANY;)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_ids_assigned_number().size() == 0);
        CHECK(cpp.get_duplicates_value_assigned_to_ids().size() == 0);
        }
    }

TEST_CASE("Qt", "[cpp][i18n]")
    {
    SECTION("tr & translate")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"( void some_global_function(LoginWidget *logwid)
 {
     QLabel *label = new QLabel(
                 LoginWidget::tr("Password:"), logwid);
 }

 void same_global_function(LoginWidget *logwid)
 {
     QLabel *label = new QLabel(
                 qApp->translate("LoginWidget", "Password:"), logwid);
 }

MyWindow::MyWindow()
{
    QLabel *senderLabel = new QLabel(tr("Name:"));
    QLabel *recipientLabel = new QLabel(tr("Name:", "recipient"));
}
QLabel *label = new QLabel(
                 s("Password:"), logwid);
)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_localizable_strings().size() == 4);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"Password:");
        CHECK(cpp.get_localizable_strings()[1].m_string == L"Password:");
        CHECK(cpp.get_localizable_strings()[2].m_string == L"Name:");
        CHECK(cpp.get_localizable_strings()[3].m_string == L"Name:");
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 3);
        // context portion of translate() and tr()
        CHECK(cpp.get_internal_strings()[0].m_string == L"LoginWidget");
        CHECK(cpp.get_internal_strings()[1].m_string == L"recipient");
        CHECK(cpp.get_internal_strings()[2].m_string == L"Password:");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("QApplication::translate")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(QString test = qApp->translate("SomeContext", "source content");
QString test = QApplication::translate("SomeContext", "more source content");)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_localizable_strings().size() == 2);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"source content");
        CHECK(cpp.get_localizable_strings()[1].m_string == L"more source content");
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 2);
        // context portion of translate()
        CHECK(cpp.get_internal_strings()[0].m_string == L"SomeContext");
        CHECK(cpp.get_internal_strings()[1].m_string == L"SomeContext");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("QT_TR_NOOP")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(QString FriendlyConversation::greeting(int type)
 {
     static const char *greeting_strings[] = {
         QT_TR_NOOP("Hello"),
         QT_TR_NOOP("Goodbye")
     };
     return tr(greeting_strings[type]);
 })";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_localizable_strings().size() == 2);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"Hello");
        CHECK(cpp.get_localizable_strings()[1].m_string == L"Goodbye");
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("QT_TRANSLATE_NOOP")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"( static const char *greeting_strings[] = {
     QT_TRANSLATE_NOOP("FriendlyConversation", "Hello"),
     QT_TRANSLATE_NOOP("FriendlyConversation", "Goodbye")
 };

 QString FriendlyConversation::greeting(int type)
 {
     return tr(greeting_strings[type]);
 }

 QString global_greeting(int type)
 {
     return qApp->translate("FriendlyConversation",
                            greeting_strings[type]);
 })";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_localizable_strings().size() == 2);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"Hello");
        CHECK(cpp.get_localizable_strings()[1].m_string == L"Goodbye");
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 3);
        CHECK(cpp.get_internal_strings()[0].m_string == L"FriendlyConversation");
        CHECK(cpp.get_internal_strings()[1].m_string == L"FriendlyConversation");
        CHECK(cpp.get_internal_strings()[2].m_string == L"FriendlyConversation");
        }
    }

TEST_CASE("Qt no-loc", "[cpp][i18n]")
    {
    SECTION("Q_MOC_INCLUDE")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(Q_MOC_INCLUDE("myheader"))"; // don't include extension
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("Q_CLASSINFO")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(class MyClass : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("Author", "Joe Smith")
    Q_CLASSINFO("URL", "http://www.my-organization.qc.ca")

public:)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 4);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Author");
        CHECK(cpp.get_internal_strings()[1].m_string == L"Joe Smith");
        CHECK(cpp.get_internal_strings()[2].m_string == L"URL");
        CHECK(cpp.get_internal_strings()[3].m_string == L"http://www.my-organization.qc.ca");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }
    }

TEST_CASE("Deprecated functions & macros", "[cpp][i18n]")
    {
    SECTION("Functions")
        {
        cpp_i18n_review cpp;
        cpp.set_style(review_style::check_deprecated_macros);
        const wchar_t* code = LR"(wxStrlenn i = 9;
auto var = ::wxStrlen(theString);
wxStrlens(var);
i = 8;
wxStrlen)";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_deprecated_macros().size() == 1);
        CHECK(cpp.get_deprecated_macros()[0].m_string == L"wxStrlen");
        CHECK(cpp.get_deprecated_macros()[0].m_column == 14);
        CHECK(cpp.get_deprecated_macros()[0].m_line == 2);
        CHECK(cpp.get_deprecated_macros()[0].m_usage.m_value == L"Use std::wcslen() or (wrap in a std::wstring_view) instead.");
        }
    }

TEST_CASE("Code generator strings", "[i18n]")
    {
    SECTION("HTML CSS style")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "<!DOCTYPE html>
<html><head>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
<title>%s - Report</title>
<link href = 'memory:master.css' rel = 'stylesheet'>
<style>
     * Sortable tables */
    table.sortable thead {cursor: default;}
    body { font-size: %s%%; }
%s
</style>
</head>
<body>")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }

    SECTION("HTML script")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "(<!DOCTYPE html>
<html><head>
<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
<title>%s - Report</title>
<link href = 'memory:master.css' rel = 'stylesheet'>
<script>
    window.Promise || document.write('<script src='memory:polyfill.min.js'><\/script>');
    window.Promise || document.write('<script src='memory:classlist.min.js'><\/script>');
    window.Promise || document.write('<script src='memory:resize-observer.js'><\/script>');
    window.Promise || document.write('<script src='memory:findindex.min.js'><\/script>');
    window.Promise || document.write('<script src='memory:umd.min.js'><\/script>');
</script>
<script src = 'memory:apexcharts.min.js'></script>
<script src = 'memory:sorttable.js'></script>
<script src = 'memory:jquery.min.js'></script>
</head>
<body>")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }

    SECTION("HTML")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "<b>Window</b><br />")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        // just one word
        code = LR"(auto var = "<thead><tr bgcolor=red><td><b>Headline</b></td></tr></thead>")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }

    SECTION("Postscript command")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "<< /Registry (Adobe)\n";)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "/DescendantFonts [%d 0 R]")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }

    SECTION("SQL command")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "INSERT INTO BILLSDEPOSITS_V1(ACCOUNTID, TOACCOUNTID";)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "CREATE INDEX IF NOT EXISTS IDX_BUDGETYEAR_BUDGETYEARNAME ON BUDGETYEAR";)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        
        code = LR"(auto var = "CREATE TABLE BUDGETTABLE_V1(BUDGETENTRYID integer primary key, BUDGETYEARID integer, CATEGID integer, PERIOD TEXT NOT NULL /* None, Weekly, Bi-Weekly, Monthly, Monthly, Bi-Monthly, Quarterly, Half-Yearly, Yearly, Daily*/, AMOUNT numeric NOT NULL, NOTES TEXT, ACTIVE integer";)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }

    SECTION("XML Tag")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "<and or>";)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "version=\"1.0\"")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "<a href=\"#scores\">")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        
        code = LR"(auto var = "<a href=\"#")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "xml version=\"1.0\"")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "xml version=\"1.0\">")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "<\?xml version=\"%s\" encoding=\"UTF-8\" standalone=\"%s\" \?>")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        // contains translatable text
        code = LR"(auto var = "xml version=\"1.0\">Hello")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);

        code = LR"(auto var = "< and or >";)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("C code")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "#ifndef _";)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "#endif // _";)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        
        code = LR"(auto var = "#define _";)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }
    }

TEST_CASE("CPP Tests", "[cpp]")
    {
    SECTION("Stream")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(s << _("Can't open file") << _("You must specify path to another database file");)";
        cpp(code, L"");
        CHECK(cpp.get_localizable_strings().size() == 2);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Dead code block")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(int Wisteria::UI::BaseApp::OnExit()
            {
            wxLogDebug(__WXFUNCTION__);
            SaveFileHistoryMenu();
            wxDELETE(m_docManager);

        #ifdef __WXMSW__
            #if 0
                // dump max memory usage
                // https://docs.microsoft.com/en-us/windows/win32/psapi/collecting-memory-usage-information-for-a-process?redirectedfrom=MSDN
                PROCESS_MEMORY_COUNTERS memCounter;
                ::ZeroMemory(&memCounter, sizeof(PROCESS_MEMORY_COUNTERS));
                if (::GetProcessMemoryInfo(::GetCurrentProcess(), &memCounter, sizeof(memCounter)))
                    {
                    const wxString memMsg = wxString::Format(L"Peak Memory Usage: %.02fGbs.",
                        safe_divide<double>(memCounter.PeakWorkingSetSize, 1024*1024*1024));
                    wxLogDebug(memMsg);
                    OutputDebugString(memMsg.wc_str());
                    }
            #elif
                const wxString memMsg = wxString::Format(L"Info: Peak Memory Usage: %.02fGbs.",
                        safe_divide<double>(memCounter.PeakWorkingSetSize, 1024*1024*1024));
                MsgBox(memMsg);
            #endif
            #ifdef _DEBUG
                MsgBox("Debug message 0!");
            #endif
            #ifndef NDEBUG
                MsgBox("Debug message 1!");
            #endif
            #ifndef NDEBUG
                MsgBox("Debug message 2!");
            #elif
                MsgBox("Release message!");
            #endif
            #if defined _DEBUG
                MsgBox("Debug message 3!");
            #endif
        #endif
            return wxApp::OnExit();
            })";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
            L"Info: Peak Memory Usage: Gbs.");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string ==
            L"Release message!");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Debug defined block")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(int Wisteria::UI::BaseApp::OnExit()
            {
            wxLogDebug(__WXFUNCTION__);
            SaveFileHistoryMenu();
            wxDELETE(m_docManager);

        #ifdef __WXMSW__
            #if wxDEBUG_LEVEL >= 2
                // dump max memory usage
                // https://docs.microsoft.com/en-us/windows/win32/psapi/collecting-memory-usage-information-for-a-process?redirectedfrom=MSDN
                PROCESS_MEMORY_COUNTERS memCounter;
                ::ZeroMemory(&memCounter, sizeof(PROCESS_MEMORY_COUNTERS));
                if (::GetProcessMemoryInfo(::GetCurrentProcess(), &memCounter, sizeof(memCounter)))
                    {
                    const wxString memMsg = wxString::Format(L"Peak Memory Usage: %.02fGbs.",
                        safe_divide<double>(memCounter.PeakWorkingSetSize, 1024*1024*1024));
                    wxLogDebug(memMsg);
                    OutputDebugString(memMsg.wc_str());
                    }
            #elif
                const wxString memMsg = wxString::Format(L"Info: Peak Memory Usage: %.02fGbs.",
                        safe_divide<double>(memCounter.PeakWorkingSetSize, 1024*1024*1024));
                MsgBox(memMsg);
            #endif
            #ifdef _DEBUG
                MsgBox("Debug message 0!");
            #endif
            #ifndef NDEBUG
                MsgBox("Debug message 1!");
            #endif
            #ifndef NDEBUG
                MsgBox("Debug message 2!");
            #elif
                MsgBox("Release message!");
            #endif
            #if defined _DEBUG
                MsgBox("Debug message 3!");
            #endif
        #endif
            return wxApp::OnExit();
            })";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
            L"Info: Peak Memory Usage: Gbs.");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string ==
            L"Release message!");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Var types to ignore")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(const LOGFONTW font("My Comic Sans");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType == L"LOGFONTW");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"font");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);

        code = LR"(LOGFONTW& font("My Comic Sans");)"; // bad syntax, but check anyway
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType == L"LOGFONTW");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"font");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);

        code = LR"(LOGFONTW const font("My Comic Sans");)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType == L"LOGFONTW");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"font");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);

        code = LR"(LOGFONTW* font = new LOGFONTW("My Comic Sans");)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        // will be seen as a function or CTOR call
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"LOGFONTW");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::function);

        code = LR"(auto font = std::make_shared<LOGFONTW>("My Comic Sans");)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"LOGFONTW");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::function);

        code = LR"(auto font = std::shared_ptr<LOGFONTW>("My Comic Sans");)";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"LOGFONTW");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::function);
        }

    SECTION("C header include")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(2);
        const wchar_t* code = LR"(auto var = "#include <wx/mstream.h>";)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "#include <vector>")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "#include \"my_header.h\"")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        code = LR"(auto var = "#include \"my_header.h\"\n\n#include <vector>\n#include <wx/mstream.h>\n")";
        cpp.clear_results();
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }

    SECTION("Separated strings")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(MessageBox("This is a long "
                                             "message across "
                                             "multiple lines");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              L"This is a long message across multiple lines");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Separated strings int 64")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(MessageBox("The amount is %0" PRId64 "\n");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              LR"(The amount is %0)");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Separated strings bad int 64 macro")
        {
        cpp_i18n_review cpp;
        // "46" is wrong, so parse this as two strings
        const wchar_t* code = LR"(MessageBox("Invalid Likert response: %\n" PRIu46
                                             "Column: %s\nValues should not exceed 7.");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              LR"(Invalid Likert response: %)");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string ==
              LR"(Column:   Values should not exceed 7.)");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Content type")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(var = "text/html; charset=utf-8")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
              L"text/html; charset=utf-8");
        }

    SECTION("File filter")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"var = \"PNG (*.png)|*.png\"";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
              L"PNG (*.png)|*.png");
        }

    SECTION("File filter word")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"var = \"Bitmap (*.bmp)|*.bmp\"";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
              L"Bitmap (*.bmp)|*.bmp");
        }

    SECTION("File filter multi extensions")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"var = \"TIFF (*.tif;*.tiff)|*.tif;*.tiff\"";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
              L"TIFF (*.tif;*.tiff)|*.tif;*.tiff");
        }

    SECTION("Printf integers")
        {
        const wchar_t* code = LR"(auto = sprintf("%zu", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%d", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%+d", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%ll", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%s", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 0);
            }
        }

    SECTION("Printf floats")
        {
        const wchar_t* code = LR"(auto = sprintf("%f", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%lf", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            CHECK(cpp.get_printf_single_numbers().size() == 1);
            }
        code = LR"(auto = sprintf("%0.4f", value))";
            {
            cpp_i18n_review cpp;
            cpp(code, L"");
            cpp.review_strings();
            // specific formatting, so std::to_string() can't replace this
            CHECK(cpp.get_printf_single_numbers().size() == 0);
            }
        }
    
    SECTION("Ignored macro")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxString GetName() const wxOVERRIDE { return wxT("Simple DirectMedia Layer"); })";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string ==
              L"Simple DirectMedia Layer");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Ignore email contact info")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = "Blake Madden <empty.name@company.mail.org>")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
            L"Blake Madden <empty.name@company.mail.org>");
        }

    SECTION("Ignore email contact info should not be l10n")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(auto var = _("Blake Madden <empty.name@company.mail.org>"))";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        }

    SECTION("Ignore email")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "emptyname@mail.org")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
            L"emptyname@mail.org");
        }

    SECTION("Not formula")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = L"auto var = _(\"%s item(s)\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 0);
        }

    SECTION("Ignore URL")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = "www.company.com")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string ==
            L"www.company.com");
        }

    SECTION("URL in l10n string")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = _("Contact us at www.company.com"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings_with_urls().size() == 1);
        }

    SECTION("Not URL in l10n string")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(auto var = _("..then by:"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings_with_urls().size() == 0);

        cpp.clear_results();
        code = LR"(auto var = _("A raster (i.e., pixel based) image format."))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings_with_urls().size() == 0);
        }

    SECTION("Internal file name")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        str = L"Log_Rep[o]rt-1.log";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"c:\\Reports\\Log_Report-1.log";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"c:\\src-stuff,files\\Log_Reporter.h";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"c:\\users\\yam\\documents&files\\audacity\\mixer\\n\\audacity\\src\\dither.cpp";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"/src/Log_Reporter.sps9";
        CHECK(cpp.is_untranslatable_string(str, false));
        // double extensions (common on UNIX files)
        str = L"dynlib.so.o";
        CHECK(cpp.is_untranslatable_string(str, false));
        // file extension
        str = L".sps9";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"*.sps9";
        CHECK(cpp.is_untranslatable_string(str, false)); // wild card
        str = L"Log-Report-1.log";
        CHECK(cpp.is_untranslatable_string(str, false));
        // ultra simple relative file path
        str = L"shaders/player1.vert";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"resources\\shaders\\player1.vert";
        CHECK(cpp.is_untranslatable_string(str, false));
        // not really a file name, the ending is deceptively like a file extension
        str = L"The maximum number of notes must be in the range 1..128";
        CHECK_FALSE(cpp.is_untranslatable_string(str, false));
        }

    SECTION("Filename")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("another.hhp"));)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == L"another.hhp");
        }

    SECTION("Filename with folder path")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("Enums/Tests.h"));)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == L"Enums/Tests.h");

        CHECK(i18n_string_util::is_file_address(L"Enums/Tests.h"));
        }

    SECTION("Place holder")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("  Lorem ipsum dolor sit amet"));)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == L"  Lorem ipsum dolor sit amet");
        }
    
    SECTION("Long place holder")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox(_("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque nisl \nmassa, luctus ut ligula vitae, suscipit tempus velit. Vivamus sodales, quam in \nconvallis posuere, libero nisi ultricies orci, nec lobortis.\n"));)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        REQUIRE(cpp.get_unsafe_localizable_strings().size() == 1);
        CHECK(cpp.get_unsafe_localizable_strings()[0].m_string == LR"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque nisl \nmassa, luctus ut ligula vitae, suscipit tempus velit. Vivamus sodales, quam in \nconvallis posuere, libero nisi ultricies orci, nec lobortis.\n)");
        }

    SECTION("Min word count")
        {
        // default is to ignore strings with just one word
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("NETHEREALM");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"NETHEREALM");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);

        // now, complain about one-word strings not exposed for l10n
        cpp.clear_results();
        cpp.set_min_words_for_classifying_unavailable_string(1);
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"NETHEREALM");
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("Min word count apos")
        {
        // default is to ignore strings with just one word
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("NETHEREALM'S");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"NETHEREALM'S");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);

        // now, complain about one-word strings not exposed for l10n
        cpp.clear_results();
        cpp.set_min_words_for_classifying_unavailable_string(1);
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"NETHEREALM'S");
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("Min word count hypthen")
        {
        // default is to ignore strings with just one word
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("part-time");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"part-time");
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);

        // now, complain about one-word strings not exposed for l10n
        cpp.clear_results();
        cpp.set_min_words_for_classifying_unavailable_string(1);
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"part-time");
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_unsafe_localizable_strings().size() == 0);
        }

    SECTION("Not filename too long")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(wxMessageBox("Microsoft Windows 2000 system level function loading error occurred during process initialization - Unable to load a function from the library file riched.dll");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"wxMessageBox");
        CHECK(cpp.get_internal_strings().size() == 0);
        }
    }

TEST_CASE("CPP Tests 2", "[cpp]")
    {
    SECTION("Hex color")
        {
        cpp_i18n_review cpp;
        std::wstring str = L"&	c #437A40";
        CHECK(cpp.is_untranslatable_string(str, false));
        }

    SECTION("Xes")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        str = L" xx xx x";
        CHECK(cpp.is_untranslatable_string(str, false));
        }

    SECTION("HTML")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = LR"(<tt><span style = 'font-weight:bold;'>)", false));
        CHECK(cpp.is_untranslatable_string(str = L"<table style=\\\"width:100%;\"><tr><td width=\"33%\">", false));
        CHECK(cpp.is_untranslatable_string(str = L"<p style=\\\"font-family: %s; font-size: %dpt; color: rgb(%u, %u, %u)\\\">\n", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n<head>\n<meta content=\"text/html; charset=UTF-8\"/>\n<title></title>\n<link href=\"stylesheet.css\" type=\"text/css\" rel=\"stylesheet\" />\n<link rel=\"stylesheet\" type=\"application/vnd.adobe-page-template+xml\" href=\"page-template.xpgt\"/>\n</head>)", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(<br />&nbsp;&nbsp;&nbsp;&nbsp;&ldquo;<span style="font-style:italic;">%s</span>&rdquo;)", false));
        }

    SECTION("Macro variable")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   ("Direct2D failed"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"DX_MSG"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("Function signature")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(void DefineProperty(const char *name, plcob pb, std::string description="my description"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"my description"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"description"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("Macro variable ignored")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   "Direct2D failed")";
        cpp.add_variable_name_pattern_to_ignore(std::wregex(L"DX_MSG"));
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == std::wstring(L"DX_MSG"));
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::variable);
        }

    SECTION("Orphan string")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(if (value =="my message"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"my message"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L""));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::orphan);
        }

    SECTION("Macro variable expands to function")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define  DX_MSG   MessageBox("Direct2D failed"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == std::wstring(L"Direct2D failed"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"MessageBox"));
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == cpp_i18n_review::string_info::usage_info::usage_type::function);
        }

    SECTION("Allow punctuation only")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        cpp.allow_translating_punctuation_only_strings(false);
        CHECK(cpp.is_untranslatable_string(str = L" % ", false));
        cpp.allow_translating_punctuation_only_strings(true);
        CHECK_FALSE(cpp.is_untranslatable_string(str = L" % ", false));
        }

    SECTION("Windows names")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = L"Windows 3.1", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows 98", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows 2000", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows NT", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows NT SP4", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows XP", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows XP SP3", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows Server", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows Server 2012", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows Server 2012 R2", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows Vista", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows 8", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows 8.1", false));
        CHECK(cpp.is_untranslatable_string(str = L"Windows 10", false));
        }

    SECTION("Unix path")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = L"/usr/myfolder/libs", false));
        CHECK(cpp.is_untranslatable_string(str = L"/usr/myfolder/libs/", false));
        CHECK(cpp.is_untranslatable_string(str = L"/usr/myfolder/libs/info.so", false));
        CHECK(cpp.is_untranslatable_string(str = L"/usr/libs/info folder", false));
        // not enough slashes to make it appear like a file path
        CHECK_FALSE(cpp.is_untranslatable_string(str = L"/usr is a root folder", false));
        }

    SECTION("Escaped quotes")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = L"description.Replace(wxT(\"\\\\\\\"\"), wxT(\"/\"));";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 2);
        CHECK(cpp.get_internal_strings()[0].m_string == L"\\\\\\\"");
        CHECK(cpp.get_internal_strings()[1].m_string == L"/");
        cpp.clear_results();
        code = L"view = (mCurTrack[0]->GetWaveformSettings().scaleType == 0) ? wxT(\"\\\"Waveform\\\"\") : wxT(\"\\\"Waveform (dB)\\\"\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"\\\"Waveform\\\"");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"\\\"Waveform (dB)\\\"");
        }

    SECTION("Quote in single quotes")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = L"if (a == '\"')\nAddCheckBox(_(\"&Use legacy (version 3) syntax.\"),\n(mVersion == 3) ? wxT(\"true\") : wxT(\"false\"));";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 1);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_localizable_strings()[0].m_string == L"&Use legacy (version 3) syntax.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"true");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"false");
        }

    SECTION("Internal Strings")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = L"", false));
        CHECK(cpp.is_untranslatable_string(str = L" ", false));
        CHECK(cpp.is_untranslatable_string(str = L"  \t", false));
        CHECK(cpp.is_untranslatable_string(str = L"1", false));
        CHECK(cpp.is_untranslatable_string(str = L"1.0", false));
        CHECK(cpp.is_untranslatable_string(str = L">", false));
        CHECK(cpp.is_untranslatable_string(str = L"> ", false));
        CHECK(cpp.is_untranslatable_string(str = L"\\n\\t\\r ", false));
        }

    SECTION("Parens")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"GetMenuBar()->SetLabel(XRCID(\"ID_SAVE_ITEM\"), wxString::Format(_(\"Export %s...\"), GetActiveProjectWindow()->GetName()) );";
        cpp(code, L"");
        cpp.review_strings();
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
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_localizable_strings().size() == 1);
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
        cpp(code, L"");
        cpp.review_strings();
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
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 3);
        CHECK(cpp.get_internal_strings()[0].m_string == L"xmlHttpRequest");
        CHECK(cpp.get_internal_strings()[1].m_string == L"supportsIpv6OnIos");
        CHECK(cpp.get_internal_strings()[2].m_string == L"xml2HttpRequest");
        }

    SECTION("Internal CSS Strings")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = L"font-style: italic;", false));
        CHECK(cpp.is_untranslatable_string(str = L"  font-style: italic;  ", false));
        CHECK(cpp.is_untranslatable_string(str = L"font-weight: bold", false));
        CHECK(cpp.is_untranslatable_string(str = L" color:red", false));
        CHECK(cpp.is_untranslatable_string(str = L"background-COLOR:red, false", false));
        CHECK(cpp.is_untranslatable_string(str = L" style = 'color: red", false));
        }

    SECTION("Internal HTML Strings")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = L"<html>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<!--commment>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<SPAN style=", false));
        CHECK_FALSE(cpp.is_untranslatable_string(str = L"<HTML>hello", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(<HTML = "hello"></html>)", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(<a href="website"><br>)", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(]]</center>\n)", false));
        CHECK(cpp.is_untranslatable_string(str = L"&amp;", false));
        CHECK(cpp.is_untranslatable_string(str = L"&#107;", false));
        CHECK(cpp.is_untranslatable_string(str = L"&#xF8;", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(charset = \"%s\"\n)", false));
        }

    SECTION("Internal XML Strings")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK_FALSE(cpp.is_untranslatable_string(str = L"<?xml>hello", false));
        CHECK(cpp.is_untranslatable_string(str = L"<?XML>7</XML>", false));
        // generic tags
        CHECK(cpp.is_untranslatable_string(str = L"<doc-val>&entity;</doc-val>", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(<comment =\")", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(<startdoctype name=\"%s\")", false));
        CHECK(cpp.is_untranslatable_string(str = LR"(<image x=%d y=\"%d\" width = '%dpx' height=\"%dpx\")", false));
        }

    SECTION("Custom XML")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = L"<ice>", false));
        CHECK(cpp.is_untranslatable_string(str = L"</ice>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<ice> 9, </ice>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<ice><ice>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<ice-level><ice-level>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<ice> <ice>", false));
        CHECK(cpp.is_untranslatable_string(str = L"<unrecognized version=\"3\">", false));
        }

    SECTION("Formulas")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        CHECK(cpp.is_untranslatable_string(str = L"=color", false));
        CHECK(cpp.is_untranslatable_string(str = L"=small", false));
        CHECK(cpp.is_untranslatable_string(str = L"Open()", false));
        CHECK(cpp.is_untranslatable_string(str = L"ABS(-2.7)", false));
        CHECK(cpp.is_untranslatable_string(str = L"POW(-4, 2)", false));
        CHECK(cpp.is_untranslatable_string(str = L"SUM(5,6)", false));
        CHECK(cpp.is_untranslatable_string(str = L"SUM(5;6)", false));
        CHECK(cpp.is_untranslatable_string(str = L"SUM(R[-4]C:R[-1]C)", false));
        CHECK(cpp.is_untranslatable_string(str = L"=SUM(R[-4]C:R[-1]C)", false));
        }

    SECTION("Variable name define")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(#define SAVE_MAGIC_V1 "fish")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"fish");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == std::wstring(L"SAVE_MAGIC_V1"));
        }

    SECTION("__asm")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"__asm int \"A\"\n  __asm {int \"B\"\nint \"C\"}\n\nif b__asm(\"this is an error\")__asm";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("asm")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"asm int \"A\"\n  asm (int \"B\"\nint \"C\")\n\nif basm(\"this is an error\")asm";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("asm volatile")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(asm volatile (                            \
                    "sd  %[val_m],  %[pdst_sd_m]  \n\t"   \
                                                          \
                    : [pdst_sd_m] "=m" (*pdst_sd_m)       \
                    : [val_m] "r" (val_m)                 \
                );if basm("this is an error"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("__asm__")
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
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("__asm__ inline")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define CATCH_TRAP()  __asm__(".inst 0xde01"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("Preprocessor Defined Variable In String Helper")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(#define REV_IDENT CString("No revision identifier was provided"))";
        cpp(code, L"");
        cpp.review_strings();
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
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = L"# include \"string\"\n#  pragma __asm \"some library\"\n#define color \"Red\"\nif ShowMessage(\"this is an error\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 2);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Red");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"this is an error");
        }

    SECTION("Preprocessor logic block")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#ifdef MYDEFINE \"bogus string\" \\ \nANOTHERDEF \"More bogus text\"\nif ShowMessage(\"this is an error\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"this is an error");
        }

    SECTION("With spaces")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//\"comment\" ASSERT() is a diagnostic function\nif assert  (true && \"this is an error\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        }

    SECTION("Assert")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"/*comment here\nand more commenting about assert(true && \"this is an error\")*/\n\nif assert(true && \"this is an error\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        }
    SECTION("Assert less than token")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"assert(delta >= 0 && \"delta value should be positive when comparing doubles\");";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"delta value should be positive when comparing doubles");
        }

    SECTION("Assert nested tokens")
        {
        cpp_i18n_review cpp;
        // check skipping over "==" to get to NON_UNIT_TEST_ASSERT
        const wchar_t* code = L"NON_UNIT_TEST_ASSERT((end1-begin1) == (end2-begin2) && \"Arrays passed to phi_coefficient must be the same size!\")";
        cpp(code, L"");
        code = L"assert(is_within<double>(pc,-1,1) && \"Error in phi coefficient calculation. Value should be -1 >= and <= 1.\")";
        cpp(code, L"");
        // check skipping over "!=" to get to NON_UNIT_TEST_ASSERT
        code = L"NON_UNIT_TEST_ASSERT((end1-begin1) != (end2-begin2) && \"Arrays passed to phi_coefficient must NOT be the same size!\")";
        cpp(code, L"");
        cpp.review_strings();
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
        cpp(code, L"");
        code = L"//comment\nif CPPUNIT_ASSERT_EQUAL(true && \"this is an error also\")";
        cpp(code, L"");
        cpp.review_strings();
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
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Arrays passed to phi_coefficient must be the same size!");
        }

    SECTION("Buried In Parentheses")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif assert(true && ( (\"this is an error\")))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"this is an error");
        }

    SECTION("Localizable In Assert")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif assert(true && _(\"this is an error\"))";
        cpp(code, L"");
        cpp.review_strings();
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
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String In CTOR variable")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"wstring message( \"Enter your ID.\" );";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"wstring");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"message");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("String In CTOR variable with braces")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"wstring message{ \"Enter your ID.\" };";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"wstring");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"message");
        CHECK(cpp.get_internal_strings().size() == 0);
        }

    SECTION("String In CTOR With Namespace")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert(ID, std::wstring(\"Enter your ID.\"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Encodings")
        {
        cpp_i18n_review cpp;
        std::wstring str;
        str = L"UTF-8";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"utf8";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"shift-jis";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"shift_jis";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"windows-1252";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"Big5";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"iso-8859-1";
        CHECK(cpp.is_untranslatable_string(str, false));
        str = L"iso-8859-13";
        CHECK(cpp.is_untranslatable_string(str, false));
        }

    SECTION("Pointless Parens")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"assert(mPipeline.get(), (\"OnPadAdded: unable to allocate stream context\"));";
        cpp(code, L"");
        cpp.review_strings();
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
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String in CTOR with global namespace")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert<char>(ID, ::basic_string(\"Enter your ID.\"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        cpp.clear_results();
        // sanity test of empty CTOR
        code = L"if assert<char>(ID, ::(\"Enter your ID.\"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Comparison in function call")
        {
        cpp_i18n_review cpp;
        // in call to assert
        const wchar_t* code = L"if assert(ID==5, \"Bad ID\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Bad ID");
        }

    SECTION("Variable assignment")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::string userMessage = \"Enter your ID.\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::string");
        }

    SECTION("Variable assignment add")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::wstring userMessage += L\"Enter your ID.\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::wstring");
        }

    SECTION("Variable assignment with pattern")
        {
        cpp_i18n_review cpp;
        cpp.add_variable_name_pattern_to_ignore(std::wregex(L"^test.*"));
        const wchar_t* code = L"std::string testMessage = \"Enter your ID.\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_internal_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"testMessage");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType == L"std::string");
        }

    SECTION("Variable assignment array")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"char userMessage [3] = \"Enter your ID.\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"char");
        }

    SECTION("Variable assignment template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::basic_string<char> userMessage = \"Enter your ID.\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        // decorations get stripped
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::basic_string");
        }

    SECTION("Variable CTOR")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::string userMessage{ \"Enter your ID.\" }";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::string");
        }

    SECTION("Variable CTOR template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::basic_string<char> userMessage{ \"Enter your ID.\" }";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        // decorations get stripped
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::basic_string");
        }

    SECTION("Variable CTOR complex template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"std::map<string, int> userMessage{ \"Enter your ID.\", 87 }";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::variable);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"userMessage");
        // decorations get stripped
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_variableType == L"std::map");
        }

    SECTION("String in parameters with other func calls")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"wxASSERT_MSG(m_defaultReadabilityTestsTemplate.get_test_count() == 0, __WXFUNCTION__ + wxString(\" called twice?\"));";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L" called twice?");
        }
    
    SECTION("Switch statement")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(switch ( GetWindowStyle() & wxBK_ALIGN_MASK )
        {
            case wxBK_TOP:
                [[fallthrough]];
            case wxBK_LEFT:
                // posCtrl is already ok
                break;
            case wxBK_BOTTOM:
                posCtrl.y = sizeClient.y - sizeNew.y;
                break;
            case wxBK_RIGHT:
                posCtrl.x = sizeClient.x - sizeNew.x;
                break;
            default:
                wxFAIL_MSG(L"unexpected alignment");
        })";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"unexpected alignment");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"wxFAIL_MSG");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType.empty());
        }
    
    SECTION("Pointers function")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(handlerInfo = m_frame->GetClassInfo()->
                FindHandlerInfo(wxT("ButtonClickHandler"));)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"ButtonClickHandler");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"FindHandlerInfo");
        CHECK(cpp.get_internal_strings()[0].m_usage.m_variableType.empty());
        CHECK(cpp.get_error_log().empty());
        }

    SECTION("Casts")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(Add(static_cast<CString*>("My ID"));)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"My ID");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_type == i18n_review::string_info::usage_info::usage_type::function);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"Add");
        }

    SECTION("String escaped")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Skip include")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#include \"string\"\n\nif GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Define variable")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"#define REV_TIME \"unknown date and time\"\n\nint i = 9";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"unknown date and time");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"REV_TIME");
        }

    SECTION("Variable initializer list")
        {
        cpp_i18n_review cpp;
        cpp.set_min_words_for_classifying_unavailable_string(1);
        const wchar_t* code = LR"(static const char * const effect_play[] = {
            "=	RR",
            "-	RR",
            ";	RR",
            ">	
    RR",
            ",	RR")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 5);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"= RR");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_string == L"- RR");
        CHECK(cpp.get_not_available_for_localization_strings()[1].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[2].m_string == L"; RR");
        CHECK(cpp.get_not_available_for_localization_strings()[2].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[3].m_string == LR"(>      RR)");
        CHECK(cpp.get_not_available_for_localization_strings()[3].m_usage.m_value == L"effect_play");
        CHECK(cpp.get_not_available_for_localization_strings()[4].m_string == L", RR");
        CHECK(cpp.get_not_available_for_localization_strings()[4].m_usage.m_value == L"effect_play");
        }

    SECTION("String escaped with escaped slash")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(wxT(\"<img src=\\\"images\\\\\"), \"Enter your \\\"ID\\\".\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_string == L"<img src=\\\"images\\\\");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("Function name pointer")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = (*env)->GetStaticMethodID(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("Function name global namespace")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = ::GetStaticMethodID(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("Function name member")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(what.Printf("standard exception of type \"%s\" with message \"%s\"");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == LR"(standard exception of type \"\" with message \"\")");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"Printf");
        }

    SECTION("Function name template")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(mid = ::GetStaticMethodID<int>(env, mActivityClass, "promptForAlias", "(II)V");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"(II)V");
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_usage.m_value == L"GetStaticMethodID");
        }

    SECTION("String commented paren")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"//comment\nif GetUserInput(/*assert(*/, \"Enter your ID.\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("String start escaped")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"blah('\\\"')\nif GetUserInput(ID, \"Enter your \\\"ID\\\".\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your \\\"ID\\\".");
        }

    SECTION("With Email")
        {
        cpp_i18n_review cpp;
        std::wstring code = LR"("An error report has been saved to : \n\"%s\".\n\nPlease email this file to support@company.com to have this issue reviewed. Thank you for your patience.")";
        CHECK_FALSE(cpp.is_untranslatable_string(code, false));
        }

    SECTION("File Filter")
        {
        cpp_i18n_review cpp;
        std::wstring code = LR"("Rich Text Format (*.rtf)|*.rtf")";
        CHECK_FALSE(cpp.is_untranslatable_string(code, false));
        }

    SECTION("String in parameters")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = L"if GetUserInput(ID, \"Enter your ID.\")";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        REQUIRE(cpp.get_not_available_for_localization_strings().size() == 1);
        CHECK(cpp.get_internal_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings()[0].m_string == L"Enter your ID.");
        }

    SECTION("Printf commands")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(DateFormat(L"%Y%m%dT%H%M%S");)";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        REQUIRE(cpp.get_internal_strings().size() == 1);
        CHECK(cpp.get_internal_strings()[0].m_usage.m_value == L"DateFormat");
        }

    SECTION("Missing space after semicolon")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"({ return wxSizerFlags::GetDefaultBorder() * 2;})";
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_error_log().size() == 1);
        CHECK(cpp.get_error_log()[0].m_message == L"Space or newline should be inserted between ';' and '}'.");
        }

    SECTION("Clipboard")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(MsgBox("\r\nStartHTML:00000000\r\nEndHTML:00000000\r\nStartFragment:00000000\r\nEndFragment:00000000\r\n<html><body>\r\n<!--StartFragment -->\r\n"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);

        cpp.clear_results();
        code = LR"(MsgBox("Version:0.9\r\nStartHTML:00000000\r\nEndHTML:00000000\r\nStartFragment:00000000\r\nEndFragment:00000000\r\n<html><body>\r\n<!--StartFragment -->\r\n"))";
        cpp(code, L"");
        cpp.review_strings();
        CHECK(cpp.get_localizable_strings().size() == 0);
        CHECK(cpp.get_not_available_for_localization_strings().size() == 0);
        CHECK(cpp.get_internal_strings().size() == 1);
        }

    SECTION("Long line")
        {
        cpp_i18n_review cpp;
        const wchar_t* code = LR"(/* Handle HTML syntax that is hard coded in the source file.
Strip it down and then see if what's left contains translatable content.
Note that we skip any punctuation (not word characters, excluding '<')
in front of the initial '<' (sometimes there are braces and brackets
in front of the HTML tags).
*/
if (std::regex_match(str, m_html_regex) ||
    std::regex_match(str, m_html_element_regex) ||
    std::regex_match(str, m_html_tag_regex) ||
    std::regex_match(str, m_html_tag_unicode_regex))
    {
    str = std::regex_replace(str,
                                            std::wregex(L"<[?]?[A-Za-z0-9+_/\\-\\.'\"=;:!%[:space:]\\\\,()]+[?]?>"), L"");
    // strip things like &ldquo;
    str = std::regex_replace(str, std::wregex(L"&[[:alpha:]]{2,5};"), L"");
    str = std::regex_replace(str, std::wregex(L"&#[[:digit:]]{2,4};"), L"");
    })";
        cpp.set_style(i18n_check::review_style::check_line_width);
        cpp(code, L"");
        cpp.review_strings();
        REQUIRE(cpp.get_wide_lines().size() == 1);
        CHECK(cpp.get_wide_lines()[0].m_usage.m_value == L"122");
        }
    }

// NOLINTEND
// clang-format on
