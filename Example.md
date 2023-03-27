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
| ../samples\subset.cpp | 281 | 38 | GreaterThanOrEqualTo | String available for translation that probably should not be in function call: _ | [suspectL10NString]
| ../samples\subset.cpp | 17 | 39 | Invalid dataset passed to column filter. | Localizable string being used within non-user facing function call: wxASSERT_MSG | [suspectL10NUsage]
| ../samples\subset.cpp | 93 | 52 | '%s': string value not found for '%s' column filter. | String not available for translation in function call: std::runtime_error | [notL10NAvailable]
| ../samples\subset.cpp |  |  |  | File contains extended ASCII characters, but is not encoding as UTF-8. | [nonUTF8File]
| ../samples\subset.cpp | 56 | 33 | '%s'— column not found for filtering. | String contains extended ASCII characters that should be encoded. | [unencodedExtASCII]

The `suspectL10NString` warning is because there is a string "GreaterThanOrEqualTo" that is inside of a `_()`
macro, making it available for translation. This does not appear to be something appropriate for
translation, hence the warning.

The `suspectL10NUsage` warning is because the string "Invalid dataset passed to column filter." is being used
in a call to `wxASSERT_MSG()`, which is a debug assert function. Asserts normally should not appear
in production releases and shouldn't be seen by end users; therefore, they should not be translated.

The `notL10NAvailable` warning is indicating that the string "'%s': string value not found for '%s' column filter." is
not wrapped in a `_()` macro and not available for localization.

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