# i18n-check

Internationalization & localization analysis system

[![cppcheck](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/Blake-Madden/i18n-check/actions/workflows/cppcheck.yml)

# Usage

i18n-check accepts the following arguments.

```shellscript
[input]: The folder to analyze.
--enable: Which checks to perform. Can be any combination of:
  all: Perform all checks (the default).
  SuspectL10NString: Check for translatable strings that don't appear that they should be (e.g., numbers, keywords, printf commands).
  suspectL10NUsage: Check for translatable strings being uses in internal contexts (e.g., debugging functions).
  notL10NAvailable: Check for strings not exposed for translation.
-i,--ignore: Folders and files to ignore (can be used multiple times).
-o,--output: The output report path. (Can either be a full path, or a file name to write to the current wording directory.)
-q,--quiet: Only print errors and the final output.
-h,--help: Print usage.
```

The following example will analyze the folder "c:\src\wxWiddgets\src" (but ignore the subfolders "expat" and "zlib"). It will only check for suspect translatable strings, and then send the output to "results.txt." 

```shellscript
i18n-check c:\src\wxWiddgets\src -i expat -i zlib --enable=SuspectL10NString -o results.txt
```
