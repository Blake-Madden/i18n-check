# i18n-check

Internationalization & localization analysis system for C++ code.

`i18n-check` scans a folder of C++ code and reviews the following issues:
-	Strings exposed for translation\* that possibly should not be. This includes (but not limited to) strings such as:
    - Filenames
    - Strings only containing `printf()` commands
    -	Numbers
    - Regular expressions
    -	Strings inside of debug functions
    - Formulas
-	Strings not available for translation that possibly should be.
-	The use of deprecated text macros (e.g., the `wxT()` macro in wxWidgets).

\* Strings are considered translatable if inside of `GETTEXT` (or related) macros. This includes `_()` and `wxTRANSLATE()`. 

[![cppcheck](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml)
[![doxygen](https://github.com/Blake-Madden/i18n-check/actions/workflows/doxygen.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/doxygen.yml)
[![unix build](https://github.com/Blake-Madden/i18n-check/actions/workflows/unix%20build.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/unix%20build.yml)
[![unit-tests](https://github.com/Blake-Madden/i18n-check/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/unit-tests.yml)

# Usage

`i18n-check` accepts the following arguments:

```shellscript
[input]: The folder to analyze.

--enable: Which checks to perform. Can be any combination of:
  all:               Perform all checks (the default).
  suspectL10NString: Check for translatable strings that shouldn't be
                     (e.g., numbers, keywords, printf commands).
  suspectL10NUsage:  Check for translatable strings being used in internal contexts
                     (e.g., debugging functions).
  notL10NAvailable:  Check for strings not exposed for translation.
  deprecatedMacros:  Check for deprecated text macros (e.g., wxT()).

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

-i,--ignore: Folders and files to ignore (can be used multiple times).

-o,--output: The output report path. (Can either be a full path, or a file name within
             the current working directory.)

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

This example will exclude multiple folders and output the results to "WDVresults.txt."

```shellscript
i18n-check C:\src\Wisteria-dataviz\src --ignore=import,i18n-check,wxsimplejson,math,easyexif,debug,utfcpp,CRCpp -o WDVresults.txt
```

# Github Action

You can also create an `i18n-check` Github action to make it part of your CI.
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
