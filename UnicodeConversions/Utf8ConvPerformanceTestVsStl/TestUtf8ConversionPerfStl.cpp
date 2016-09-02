////////////////////////////////////////////////////////////////////////////////
//
// Testing Unicode UTF-16 to UTF-8 encoding conversions
// using Win32 API calls vs. C++ standard library classes.
//
// by Giovanni Dicanio
//
////////////////////////////////////////////////////////////////////////////////

#include <crtdbg.h>     // For _ASSERTE

#include <algorithm>
#include <codecvt>
#include <iostream>
#include <locale>
#include <random>
#include <string>
#include <vector>

#include "Stopwatch.h"
#include "../utf8conv/utf8conv.h"

using namespace std;


vector<wstring> BuildTestStrings()
{
    const wstring lorem[] =
    {
        L"Lorem ipsum dolor sit amet, consectetuer adipiscing elit.",
        L"Maecenas porttitor congue massa. Fusce posuere, magna sed",
        L"pulvinar ultricies, purus lectus malesuada libero,",
        L"sit amet commodo magna eros quis urna.",
        L"Nunc viverra imperdiet enim. Fusce est. Vivamus a tellus.",
        L"Pellentesque habitant morbi tristique senectus et netus et",
        L"malesuada fames ac turpis egestas. Proin pharetra nonummy pede.",
        L"Mauris et orci."
    };

    vector<wstring> v;

#ifdef _DEBUG
    static const int count = 10;
#else
    static const int count = 200'000;
#endif
    for (int i = 0; i < count; ++i)
    {
        for (auto& s : lorem)
        {
            v.push_back(s + L" (#" + to_wstring(i) + L")");
        }
    }

    mt19937 prng{ 1995 };

    shuffle(v.begin(), v.end(), prng);

    return v;
}


int main()
{
    const vector<wstring> strings = BuildTestStrings();
    win32::Stopwatch sw;

    // 
    // Conversion using STL's codecvt
    //
    sw.Start();
    wstring_convert<codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
    vector<string> v1;
    v1.reserve(strings.size());
    for (const auto& s : strings)
    {
        v1.push_back(conversion.to_bytes(s));
    }
    sw.Stop();
    cout << "codecvt: " << sw.ElapsedMilliseconds() << " ms\n";

    //
    // Conversion using custom Win32 API calls
    //
    sw.Start();
    vector<string> v2;
    v2.reserve(strings.size());
    for (const auto& s : strings)
    {
        v2.push_back(win32::Utf16ToUtf8(s));
    }
    sw.Stop();
    cout << "Win32 API calls: " << sw.ElapsedMilliseconds() << " ms\n";
    
    // Correctness check in debug builds
    _ASSERTE(v1 == v2);
}
