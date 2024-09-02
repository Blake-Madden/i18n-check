<img src="app-logo.svg" width="150" />

i18n-check
=============================

Internationalization & localization analysis system for C++ code.

`i18n-check` is a command-line utility that scans a folder of C++ code and checks for the following issues:
- Strings exposed for translation¹ that probably should not be. This includes (but not limited to):
  - Filenames
  - Strings only containing `printf()` commands
  - Numbers
  - Regular expressions
  - Strings inside of debug functions
  - Formulas
  - Code (used for code generators)
  - Strings that contain URLs or email addresses
- Strings not available for translation that possibly should be.
- Strings that contain extended ASCII characters that are not encoded.
  ("Danke schön" instead of "Danke sch\U000000F6n".)<br />
  Encoding extended ASCII characters is recommended for
  best portability between compilers.
- Strings with malformed syntax (e.g., malformed HTML tags).
- Use of deprecated text macros (e.g., `wxT()` in wxWidgets, `_T()` in Win32).
- Use of deprecated string functions (e.g., `_tcsncpy` in Win32).
- Files that contain extended ASCII characters, but are not UTF-8 encoded.<br />
  (It is recommended that files be UTF-8 encoded for portability between compilers.)
- UTF-8 encoded files which start with a BOM/UTF-8 signature.<br />
  It is recommended to save without the file signature for best compiler portability.
- `printf()`-like functions being used to just format an integer to a string.<br />
  It is recommended to use `std::to_string()` to do this instead.
- `printf()` command mismatches between source and translation strings.<br />
  (PO catalogs with C/C++ strings are currently supported.)
- ID variable² assignment issues:
  - The same value being assigned to different ID variables in the same source file
    (e.g., "wxID_HIGHEST + 1" being assigned to two menu ID constants).
  - Hard-coded numbers being assigned to ID variables.
  - Out-of-range values being assigned to MFC IDs.
- Font issues in Windows resource files:
  - Dialogs not using "MS Shell Dlg" or "MS Shell Dlg 2."
  - Dialogs with non-standard font sizes.

Code formatting issues can also be checked for, such as:
- Trailing spaces at the end of a line.
- Tabs (instead of spaces).
- Lines longer than 120 characters.
- Spaces missing between "//" and their comments.

`i18n-check` will work with C, modern C++, and '98/'03 C++ code, GNU *gettext* translation files (\*.po), and Windows resource files (\*.rc).
In particular, it offers specialized support for the following frameworks:

- wxWidgets
- Qt
- KDE
- GTK
- Win32
- MFC

¹ Strings are considered translatable if inside of [gettext](https://www.gnu.org/software/gettext/),
[wxWidgets](https://www.wxwidgets.org), [Qt](https://www.qt.io), or
[KDE](https://develop.kde.org/docs/plasma/widget/translations-i18n) (ki18n) i18n functions.
This includes functions and macros such as `gettext()`, `_()`, `tr()`, `translate()`,
`QT_TR_NOOP()`, `wxTRANSLATE()`, `i18n()`, etc.<br />
² Variables are determined to be ID variables if they are integral types with the whole word "ID" in their name.

Refer [here](Example.md) for example usage.

[![Linux build](https://github.com/Blake-Madden/i18n-check/actions/workflows/unix%20build.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/linux-build.yml)
[![macOS build](https://github.com/Blake-Madden/i18n-check/actions/workflows/macOS%20build.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/macOS%20build.yml)
[![Windows build](https://github.com/Blake-Madden/i18n-check/actions/workflows/msw-build.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/msw-build.yml)

[![unit-tests](https://github.com/Blake-Madden/i18n-check/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/unit-tests.yml)
[![macOS unit tests](https://github.com/Blake-Madden/i18n-check/actions/workflows/macos-unit-test.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/macos-unit-test.yml)
[![Windows unit tests](https://github.com/Blake-Madden/i18n-check/actions/workflows/msw-unit-tests.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/msw-unit-tests.yml)

[![cppcheck](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml)
[![CodeQL](https://github.com/Blake-Madden/i18n-check/actions/workflows/codeql.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/codeql.yml)
[![Microsoft C++ Code Analysis](https://github.com/Blake-Madden/i18n-check/actions/workflows/msvc.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/msvc.yml)

[![doxygen](https://github.com/Blake-Madden/i18n-check/actions/workflows/doxygen.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/doxygen.yml)
[![Check Spelling](https://github.com/Blake-Madden/i18n-check/actions/workflows/spell-check.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/spell-check.yml)
[!i18n-check](https://github.com/Blake-Madden/i18n-check/actions/workflows/unix%20build.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/i18n-check.yml)

# Usage

`i18n-check` accepts the following arguments:

```shellscript
[input]: The folder to analyze.

--enable: Which checks to perform. Can be any combination of:
  allI18N:             Perform all internationalization checks (the default).
  allL10N:             Perform all localization checks (the default).
  allCodeFormatting:   Check all code formatting issues.
                       These are not enabled by default.
  suspectL10NString:   Check for translatable strings that shouldn't be
                       (e.g., numbers, keywords, printf() commands).
  suspectL10NUsage:    Check for translatable strings being used in internal contexts
                       (e.g., debugging functions).
  urlInL10NString:     Check for translatable strings that contain URLs or email addresses.
                       It is recommended to dynamically format these into the string so that
                       translators don't have to manage them.
  notL10NAvailable:    Check for strings not exposed for translation.
  deprecatedMacro:     Check for deprecated text macros (e.g., wxT()).
  nonUTF8File:         Check that files containing extended ASCII characters are UTF-8 encoded.
  UTF8FileWithBOM:     Check for UTF-8 encoded files which start with a BOM/UTF-8 signature.
                       It is recommended to save without the file signature for
                       best compiler portability.
  unencodedExtASCII:   Check for strings containing extended ASCII characters that are not encoded.
  printfSingleNumber:  Check for printf()-like functions being used to just format a number.
  dupValAssignedToIds: Check for the same value being assigned to different ID variables.
  numberAssignedToId:  Check for ID variables being assigned a hard-coded number.
                       It may be preferred to assign framework-defined constants to IDs.
  malformedString:     Check for malformed syntax in strings (e.g., malformed HTML tags).
  fontIssue:           Check for font issues (e.g., Windows *.RC dialogs not using MS Shell Dlg
                       or using unusual font sizes).
  trailingSpaces:      Check for trailing spaces at the end of each line.
                       This is a code formatting check and is not enabled by default.
  tabs:                Check for tabs.
                       (Spaces are recommended as tabs may appear differently between editors.)
                       This is a code formatting check and is not enabled by default.
  wideLine:            Check for overly long lines.
                       This is a code formatting check and is not enabled by default.
  commentMissingSpace: Check that there is a space at the start of a comment.
                       This is a code formatting check and is not enabled by default.
  printfMismatch:      Check for mismatching printf() commands between source and translation strings.

--disable: Which checks to not perform. (Refer to options available above.)
           This will override any options passed to "--enable".

--log-l10n-allowed: Whether it is acceptable to pass translatable strings to 
                    logging functions. Setting this to false will emit warnings
                    when a translatable string is passed to functions such as
                    wxLogMessage or SDL_Log.
                    (Default is true.)

--punct-l10n-allowed: Whether it is acceptable for punctuation only strings to be
                      translatable. Setting this to true will suppress warnings about
                      strings such as " - " being available for localization.
                      (Default is false.)

--exceptions-l10n-required: Whether to verify that exception messages are available
                            for translation. Setting this to true will emit warnings
                            when untranslatable strings are passed to various exception
                            constructors or functions (e.g., AfxThrowOleDispatchException).
                            (Default is true.)

--min-l10n-wordcount: The minimum number of words that a string must have to be
                      considered translatable. Higher values for this will result in less
                      strings being classified as a notL10NAvailable warning.
                      (Default is 2.)

--fuzzy:              Whether to review fuzzy translations.
                      (Default is false.)

--cpp-version: The C++ standard that should be assumed when issuing deprecated macro warnings.
               (Default is 14.)

-i,--ignore: Folders and files to ignore (can be used multiple times).

-o,--output: The output report path (tab-delimited format). (Can either be a full path,
             or a file name within the current working directory.)

-q,--quiet: Only print errors and the final output.

-v,--verbose: Display debug information.

-h,--help: Print usage.
```

The following example will analyze the folder "c:\src\wxWidgets\src"
(but ignore the subfolders "expat" and "zlib"). It will only check for
suspect translatable strings, and then send the output to "results.txt"
in the current working directory. 

```shellscript
i18n-check C:\src\wxWidgets\src -i expat -i zlib --enable=suspectL10NString -o results.txt
```

This example will only check for `suspectL10NUsage` and `suspectL10NString` and not show
any progress messages.

```shellscript
i18n-check C:\src\wxWidgets\samples -q --enable=suspectL10NUsage,suspectL10NString
```

This example will ignore multiple folders (and files) and output the results to "WDVResults.txt."

```shellscript
i18n-check C:\src\Wisteria-dataviz\src --ignore=import,i18n-check,wxsimplejson,wxStartPage,math,easyexif,debug,utfcpp,CRCpp,base/colors.cpp,base/colors.h -o WDVresults.txt
```

Refer [here](Example.md) for more examples.

# Building (command line tool)

`i18n-check` can be configured and built with *Cmake*.

On Unix:

```shellscript
cmake .
cmake --build . --target all -j $(nproc)
```

On Windows, "CMakeLists.txt" can be opened and built directly in Visual Studio.

After building, "i18n-check" will then be available in the "bin" folder.

# Building (GUI)

[wxWidgets](https://github.com/wxWidgets/wxWidgets) 3.3 or higher is required for building the graphical user interface version for `i18n-check`.

Download [wxWidgets](https://github.com/wxWidgets/wxWidgets), placing it at the same folder level as `i18n-check`:

```
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
```

Refer [here](https://github.com/wxWidgets/wxWidgets/blob/master/README-GIT.md) for how to build wxWidgets.

After building wxWidgets, `ii18n-gui` can be configured and built with *Cmake*.

On Unix:

```shellscript
cd gui
cmake .
cmake --build . --target all -j $(nproc)
```

On Windows, "gui/CMakeLists.txt" can be opened and built directly in Visual Studio.

After building, "ii18n-gui" will then be available in the "bin" folder.

# GitHub Action

You can also create an `i18n-check` GitHub action to make it part of your CI.
For example, create a new workflow called "i18n-check.yml" and enter the following:

```shellscript
name: i18n-check
on: [push]

jobs:
  build:
    name: i18n-check
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: install i18n-check
        run: |
             git clone https://github.com/Blake-Madden/i18n-check.git --recurse-submodules
             cd i18n-check
             cmake ./
             make -j4
             cd ..

      - name: analyze
        run: |
             # Ignore i18n-check's own folder.
             # You can ignore other folders by adding a comma and the folder name
             # after "--ignore=i18n-check".
             ./i18n-check/bin/i18n-check ./ --ignore=i18n-check -q -o i18nresults.txt

      - name: review results
        run: |
             REPORTFILE=./i18nresults.txt
             WARNINGSFILE=./warnings.txt
             if test -f "$REPORTFILE"; then
                cat "$REPORTFILE" > "$WARNINGSFILE"
                # are there any warnings?
                if grep -qP '\[[a-zA-Z0-9]+\]' "$WARNINGSFILE"; then
                    # print the remaining warnings
                    echo Warnings detected:
                    echo ==================
                    cat "$WARNINGSFILE" | grep -P '\[[a-zA-Z0-9]+\]'
                    # fail the job
                    exit 1
                else
                    echo No issues detected
                fi
             else
                echo "$REPORTFILE" not found
             fi
```
