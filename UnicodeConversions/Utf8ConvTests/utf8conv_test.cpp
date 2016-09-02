////////////////////////////////////////////////////////////////////////////////
//
// utf8conv_test.cpp -- Copyright (C) by Giovanni Dicanio
//
// Test cases for the UTF-8 encoding conversion functions.
//
////////////////////////////////////////////////////////////////////////////////


#include "../utf8conv/utf8conv.h"   // UTF-8 conversion functions to test

#include <exception>    // For std::exception
#include <iostream>     // For console output
#include <stdexcept>    // For std::overflow_error
#include <string>       // For std::string and std::wstring


//
// Tests with gigantic strings (whose lengths expressed in size_t can't fit
// into an int and can't be passed to the MultiByteToWideChar 
// and WideCharToMultiByte Win32 APIs) enabled by default in 64-bit builds.
//
// You can override that commenting out the following lines:
//
#ifdef _WIN64
#define TEST_GIGANTIC_STRINGS
#endif


namespace
{

//------------------------------------------------------------------------------
// Helper function to print a test error message.
// Don't use it in test case code (use the TEST_ERROR macro instead).
//------------------------------------------------------------------------------
void PrintTestError(const char* const file, const int line, const std::string& msg)
{
    std::cout << "[ERROR] " << file << '(' << line << "): " << msg << '\n';
}

} // anonymous namespace


//------------------------------------------------------------------------------
// Macro to print test error messages, and increase the global error count.
// Use it in test cases to log failed tests.
//------------------------------------------------------------------------------
#define TEST_ERROR(msg)                             \
    do                                              \
    {                                               \
        ++g_testErrors;                             \
        PrintTestError(__FILE__, __LINE__, (msg));  \
    }                                               \
    __pragma(warning(suppress:4127)) while (0)


// Count of test errors
static int g_testErrors = 0;

// Entry point for tests
void RunTests();


//------------------------------------------------------------------------------
// Test console application's entry point
//------------------------------------------------------------------------------
int main()
{
    static const int kExitOk = 0;
    static const int kExitError = 1;
    int exitCode = kExitOk;

    try
    {
        std::cout << "\nTesting UTF-8 STL String Conversion Helpers\n"
                  << "  -- by Giovanni Dicanio\n\n";
        RunTests();
    }
    catch (const win32::Utf8ConversionException& ex)
    {
        std::cout << "\n*** FATAL: Utf8ConversionException caught:\n"
            << ex.what() << '\n'
            << "Error code = " << ex.ErrorCode() << '\n';
        exitCode = kExitError;
    }
    catch (const std::exception& ex)
    {
        std::cout << "\n*** FATAL: C++ std::exception caught:\n"
            << ex.what() << '\n';
        exitCode = kExitError;
    }
    catch (...)
    {
        std::cout << "\n*** FATAL: Unknown C++ exception caught.\n";
        exitCode = kExitError;
    }

    if (g_testErrors != 0)
    {
        std::cout << "\n*** " << g_testErrors << " error(s) detected.\n";
        exitCode = kExitError;
    }

    if (exitCode == kExitOk)
    {
        // All right!! :)
        std::cout << "\n*** No errors detected! :) ***\n";
    }

    return exitCode;
}


//------------------------------------------------------------------------------
// Various Tests
//------------------------------------------------------------------------------

void TestBasicConversionsWithStlStrings()
{
    std::wstring s1u16 = L"Hello world";
    std::string s1u8 = win32::Utf16ToUtf8(s1u16);
    std::wstring s1u16back = win32::Utf8ToUtf16(s1u8);
    if (s1u16back != s1u16)
    {
        TEST_ERROR("Converting from UTF-16 to UTF-8 and back gives different string.");
    }

    std::string s2u8 = "Ciao ciao";
    std::wstring s2u16 = win32::Utf8ToUtf16(s2u8);
    std::string s2u8back = win32::Utf16ToUtf8(s2u16);
    if (s2u8back != s2u8)
    {
        TEST_ERROR("Converting from UTF-8 to UTF-16 and back gives different string.");
    }
}


void TestBasicConversionWithRawPointers()
{
    const wchar_t* const s1u16 = L"Hello world";
    std::string s1u8 = win32::Utf16ToUtf8(s1u16);
    std::wstring s1u16back = win32::Utf8ToUtf16(s1u8);
    if (s1u16back != s1u16)
    {
        TEST_ERROR("Converting raw pointers from UTF-16 to UTF-8 and back gives different string.");
    }

    const char* const s2u8 = "Ciao ciao";
    std::wstring s2u16 = win32::Utf8ToUtf16(s2u8);
    std::string s2u8back = win32::Utf16ToUtf8(s2u16);
    if (s2u8back != s2u8)
    {
        TEST_ERROR("Converting raw pointers from UTF-8 to UTF-16 and back gives different string.");
    }
}


void TestEmptyStringConversions()
{
    const std::wstring u16empty;
    const std::string u8empty;

    if (!win32::Utf16ToUtf8(u16empty).empty())
    {
        TEST_ERROR("Empty UTF-16 string is not converted to an empty UTF-8.");
    }

    if (!win32::Utf8ToUtf16(u8empty).empty())
    {
        TEST_ERROR("Empty UTF-8 string is not converted to an empty UTF-16.");
    }

    if (!win32::Utf16ToUtf8(L"").empty())
    {
        TEST_ERROR("Empty UTF-16 raw string ptr is not converted to an empty UTF-8.");
    }

    if (!win32::Utf8ToUtf16("").empty())
    {
        TEST_ERROR("Empty UTF-8 raw string ptr is not converted to an empty UTF-16.");
    }
}


void TestJapaneseKin()
{
    //
    // Test "kin"
    // UTF-16: 91D1
    // UTF-8:  E9 87 91
    //

    const std::string kinU8 = "\xE9\x87\x91";
    const std::wstring kinU16 = L"\x91D1";
    if (win32::Utf8ToUtf16(kinU8) != kinU16)
    {
        TEST_ERROR("Converting Japanese 'kin' from UTF-8 to UTF-16 failed.");
    }

    if (win32::Utf16ToUtf8(kinU16) != kinU8)
    {
        TEST_ERROR("Converting Japanese 'kin' from UTF-16 to UTF-8 failed.");
    }
}


void TestInvalidUnicodeSequences()
{
    try
    {
        // String containing invalid UTF-8
        const std::string invalidUtf8 = "Invalid UTF-8 follows: \xC0\x76\x77";

        // The following line should throw because of invalid UTF-8 sequence
        // in input string
        std::wstring invalidUtf16 = win32::Utf8ToUtf16(invalidUtf8);

        TEST_ERROR("win32::Utf8ConversionException not thrown in presence of invalid UTF-8.");
    }
    catch (const win32::Utf8ConversionException& utf8error)
    {
        if (utf8error.ErrorCode() != ERROR_NO_UNICODE_TRANSLATION)
        {
            TEST_ERROR("Error code different than ERROR_NO_UNICODE_TRANSLATION.");
        }

        if (utf8error.Direction() != win32::Utf8ConversionException::ConversionType::FromUtf8ToUtf16)
        {
            TEST_ERROR("Wrong conversion type stored in exception class.");
        }
    }

    try
    {
        // String containing invalid UTF-16
        const std::wstring invalidUtf16 = L"Invalid UTF-16: \xD800\x0100";

        // The following line should throw because of invalid UTF-16 sequence
        // in input string
        std::string invalidUtf8 = win32::Utf16ToUtf8(invalidUtf16);

        TEST_ERROR("win32::Utf8ConversionException not thrown in presence of invalid UTF-16.");
    }
    catch (const win32::Utf8ConversionException& utf8error)
    {
        if (utf8error.ErrorCode() != ERROR_NO_UNICODE_TRANSLATION)
        {
            TEST_ERROR("Error code different than ERROR_NO_UNICODE_TRANSLATION.");
        }

        if (utf8error.Direction() != win32::Utf8ConversionException::ConversionType::FromUtf16ToUtf8)
        {
            TEST_ERROR("Wrong conversion type stored in exception class.");
        }
    }
}


#ifdef TEST_GIGANTIC_STRINGS
void TestGiganticStrings()
{
    try
    {
        constexpr size_t giga = 1ULL * 1024 * 1024 * 1024;
        const std::string hugeUtf8(5 * giga, 'C');
        std::wstring hugeUtf16 = win32::Utf8ToUtf16(hugeUtf8);

        TEST_ERROR("std::overflow_error not thrown in presence of UTF-8 string whose length can't fit into an int.");
    }
    catch (const std::overflow_error& ex)
    {
        // All right
        std::cout << "\nHuge UTF-8 string throwing std::overflow_error as expected:\n"
            << ex.what() << "\n";
    }

    try
    {
        constexpr size_t giga = 1ULL * 1024 * 1024 * 1024;
        const std::wstring hugeUtf16(3 * giga, L'C');
        std::string hugeUtf8 = win32::Utf16ToUtf8(hugeUtf16);

        TEST_ERROR("std::overflow_error not thrown in presence of UTF-16 string whose length can't fit into an int.");
    }
    catch (const std::overflow_error& ex)
    {
        // All right
        std::cout << "\nHuge UTF-16 string throwing std::overflow_error as expected:\n"
            << ex.what() << "\n";
    }
}
#endif // TEST_GIGANTIC_STRINGS

void RunTests()
{
    TestBasicConversionsWithStlStrings();
    TestBasicConversionWithRawPointers();
    TestEmptyStringConversions();
    TestJapaneseKin();
    TestInvalidUnicodeSequences();

#ifdef TEST_GIGANTIC_STRINGS
    TestGiganticStrings();
#endif // TEST_GIGANTIC_STRINGS
}

