# Example

After building, go into the "bin" folder and run this command
to analyze the sample file:

```shellscript
i18n-check ../samples -o results.txt
```

This will produce a "results.txt" file in the "bin" folder with
output like this:

| File  | Line | Column | Value| Explanation | WarningID |
|-----------|-----------|-----------|-----------|-----------|-----------|
| ../samples/subset.cpp | 281 | 38 | "GreaterThanOrEqualTo" | String available for translation that probably should not be in function call: _ | [suspectL10NString]
| ../samples/subset.cpp | 17 | 39 | "Invalid dataset passed to column filter." | Localizable string being used within non-user facing function call: wxASSERT_MSG | [suspectL10NUsage]
| ../samples/subset.cpp | 93 | 52 | "'%s': string value not found for '%s' column filter." | String not available for translation in function call: std::runtime_error | [notL10NAvailable]
| ../samples/subset.cpp | 131 | 38 | wxT | Deprecated text macro that can be removed. (Add 'L' in front of string to make it double-byte.) | [deprecatedMacro]
| ../samples/subset.cpp |  |  |  | File contains extended ASCII characters, but is not encoding as UTF-8. | [nonUTF8File]
| ../samples/subset.cpp | 56 | 33 | "'%s'— column not found for filtering." | String contains extended ASCII characters that should be encoded. | [unencodedExtASCII]

The `suspectL10NString` warning is because there is a string "GreaterThanOrEqualTo" that is inside of a `_()`
macro, making it available for translation. This does not appear to be something appropriate for
translation, hence the warning.

The `suspectL10NUsage` warning is because the string "Invalid dataset passed to column filter." is being used
in a call to `wxASSERT_MSG()`, which is a debug assert function. Asserts normally should not appear
in production releases and shouldn't be seen by end users; therefore, they should not be translated.

The `notL10NAvailable` warning is indicating that the string "'%s': string value not found for '%s' column filter." is
not wrapped in a `_()` macro and not available for localization.

The `deprecatedMacro` warning is indicating that the text-wrapping macro `wxT()` should be removed.

The `nonUTF8File` warning is indicating that the file contains extended ASCII characters, but
is not encoded as UTF-8. It is generally recommended to encode files as UTF-8, making them portable between compilers and other tools.

The `unencodedExtASCII` warning is indicating that the string "'%s'— column not found for filtering." contains a hard-coded
extended ASCII character. It is recommended that these characters be encoded in hexadecimal format to avoid
character-encoding issues between compilers.

To look only for suspect strings that are exposed for translation and show the results
in the console window:

```shellscript
i18n-check ../samples --enable=suspectL10NString,suspectL10NUsage
```

To look for all issues except for deprecated macros:

```shellscript
i18n-check ../samples --disable=deprecatedMacros
```

By default, `i18n-check` will assume that messages inside of various exceptions should be translatable.
If these messages are not exposed for localization, then a warning will be issued.

To consider exception messages as internal (and suppress warnings about their messages not being localizable)
do the following:

```shellscript
i18n-check ../samples --exceptions-l10n-required=false
```

Similarity, `i18n-check` will also consider messages inside of various logging functions to be allowable
for translation. A difference is that it will not warn if a message is not exposed for translation. This is because
log messages can serve a dual role of user-facing messages and internal messages meant for developers.

To consider all log messages to never be appropriate for translation, do the following:

```shellscript
i18n-check ../samples --log-l10n-allowed=false
```

To display any code-formatting issues, enable them explicitly:

```shellscript
i18n-check ../samples --enable=allI18N,trailingSpaces,tabs,wideLine
```

or

```shellscript
i18n-check ../samples --enable=allI18N,allCodeFormatting
```

This will emit the following warnings:

| File  | Line | Column | Value| Explanation | WarningID |
|-----------|-----------|-----------|-----------|-----------|-----------|
| ../samples/subset.cpp | 18 | 1 | "" | Tab detected in file; prefer using spaces. | [tabs]
| ../samples/subset.cpp | 30 | 61 | "m_comparisonType = subsetCriterion.m_comparisonType;" | Trailing space(s) detected at end of line. | [trailingSpaces]
| ../samples/subset.cpp | 31 | 12 | "" | Trailing space(s) detected at end of line. | [trailingSpaces]
| ../samples/subset.cpp | 57 | 69 | "subsetCriterion.m_columnName).ToUTF8());" | Trailing space(s) detected at end of line. | [trailingSpaces]
| ../samples/subset.cpp | 80 | 123 | "                                                        wxString::Format(_(L"                                           )," | Line is 123 characters long. | [wideLine]
