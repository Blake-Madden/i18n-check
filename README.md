# i18n-check

Internationalization & localization analysis system for C++ code.

[![cppcheck](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml)
[![doxygen](https://github.com/Blake-Madden/i18n-check/actions/workflows/doxygen.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/doxygen.yml)

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
i18n-check c:\src\wxWidgets\src -i expat -i zlib
           --enable=SuspectL10NString -o results.txt
```

This example will only check for `suspectL10NUsage` and `suspectL10NString` and not show
any progress messages.

```shellscript
i18n-check c:\src\wxWidgets\samples -q --enable=suspectL10NUsage,suspectL10NString
```
