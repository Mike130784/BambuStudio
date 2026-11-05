// SPDX-License-Identifier: AGPL-3.0-or-later
// Test-only renderer stand-in: exercises the real GUI's 60-second deadline
// without expensive geometry. Not OpenSCAD; never bundled with the app.
#include <windows.h>
#include <fstream>
#include <string>
int wmain(int argc, wchar_t **argv)
{
    if (argc != 10 || std::wstring(argv[1]) != L"--export-format" ||
        std::wstring(argv[2]) != L"binstl" || std::wstring(argv[3]) != L"-o" ||
        std::wstring(argv[5]) != L"-p" || std::wstring(argv[7]) != L"-P" ||
        std::wstring(argv[8]) != L"custom") return 87;
    std::ofstream marker("timeout-probe.pid");
    marker << GetCurrentProcessId() << '\n'; marker.close();
    if (!marker) return 86;
    Sleep(90000);
    return 88; // If the GUI deadline fails, still exit without producing an STL.
}
