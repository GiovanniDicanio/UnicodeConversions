# UnicodeConversions

## C++ code implementing Unicode UTF-8/UTF-16 encoding conversions using Win32 APIs

by Giovanni Dicanio (e-mail: `<giovanni.dicanio AT gmail.com>`)

This repository contains C++ reusable code implementing Unicode encoding conversions between UTF-8 and UTF-16.

The reusable conversion code is implemented as a header-only library, and can be found in the subfolder named [`utf8conv`](https://github.com/GiovanniDicanio/UnicodeConversions/tree/master/UnicodeConversions/utf8conv).

This code is based on [my MSDN Magazine article](https://msdn.microsoft.com/magazine/mt763237) published on the 2016 September issue:

> [C++ - Unicode Encoding Conversions with STL Strings and Win32 APIs](https://msdn.microsoft.com/magazine/mt763237)

This code uses STL strings (`std::string` for UTF-8 and `std::wstring` for UTF-16) and direct calls to the Win32 APIs `MultiByteToWideChar()` and `WideCharToMultiByte()`, and C++ exceptions to report conversion errors (more details can be found in the [`utf8conv.h`](https://github.com/GiovanniDicanio/UnicodeConversions/blob/master/UnicodeConversions/utf8conv/utf8conv.h) header).

The included Visual Studio 2015 solution contains a simple _unit-test_ and a _performance benchmark_ testing this custom code vs. standard C++ Unicode encoding converters.

The code has been written to be usable also in VS2010 C++ projects (so no VS2015-specific C++ features like brace-init or `constexpr` have been used).
