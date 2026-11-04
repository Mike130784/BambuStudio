// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#ifdef _WIN32
#include <windows.h>
#include <string>
#include <vector>
#include <stdexcept>

namespace Slic3r::GUI::Parametric {
// Each argument is quoted independently; no command shell is involved.
inline std::wstring quote_argument(const std::wstring &s)
{
    std::wstring out = L"\"";
    size_t slashes = 0;
    for (wchar_t c : s) {
        if (c == L'\\') { ++slashes; continue; }
        out.append(slashes * (c == L'"' ? 2 : 1), L'\\');
        slashes = 0;
        if (c == L'"') out += L'\\';
        out += c;
    }
    out.append(slashes * 2, L'\\');
    return out + L'"';
}

class WindowsRenderJob {
    HANDLE m_job = nullptr, m_process = nullptr;
public:
    WindowsRenderJob() = default;
    WindowsRenderJob(const WindowsRenderJob&) = delete;
    WindowsRenderJob& operator=(const WindowsRenderJob&) = delete;
    ~WindowsRenderJob() { stop(); }
    void stop() noexcept {
        if (m_job) { TerminateJobObject(m_job, 1); CloseHandle(m_job); m_job = nullptr; }
        if (m_process) { CloseHandle(m_process); m_process = nullptr; }
    }
    bool finished(DWORD &code) const {
        if (!m_process || WaitForSingleObject(m_process, 0) != WAIT_OBJECT_0) return false;
        if (!GetExitCodeProcess(m_process, &code)) throw std::runtime_error("Cannot read renderer exit status.");
        return true;
    }
    void start(const std::wstring &exe, const std::vector<std::wstring> &args,
               const std::wstring &cwd, const std::wstring &log)
    {
        stop();
        SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        HANDLE output = CreateFileW(log.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &security,
                                    CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        HANDLE input = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   &security, OPEN_EXISTING, 0, nullptr);
        if (output == INVALID_HANDLE_VALUE || input == INVALID_HANDLE_VALUE) {
            if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
            if (input != INVALID_HANDLE_VALUE) CloseHandle(input);
            throw std::runtime_error("Cannot create private renderer streams.");
        }
        m_job = CreateJobObjectW(nullptr, nullptr);
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
        limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        if (!m_job || !SetInformationJobObject(m_job, JobObjectExtendedLimitInformation, &limits, sizeof(limits))) {
            CloseHandle(output); CloseHandle(input); stop();
            throw std::runtime_error("Cannot isolate renderer lifetime.");
        }
        SIZE_T bytes = 0;
        InitializeProcThreadAttributeList(nullptr, 1, 0, &bytes);
        std::vector<unsigned char> storage(bytes);
        auto attributes = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(storage.data());
        bool initialized = InitializeProcThreadAttributeList(attributes, 1, 0, &bytes) != FALSE;
        HANDLE streams[] = {input, output};
        bool configured = initialized && UpdateProcThreadAttribute(attributes, 0,
            PROC_THREAD_ATTRIBUTE_HANDLE_LIST, streams, sizeof(streams), nullptr, nullptr);
        STARTUPINFOEXW startup{};
        startup.StartupInfo.cb = sizeof(startup);
        startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
        startup.StartupInfo.hStdInput = input;
        startup.StartupInfo.hStdOutput = startup.StartupInfo.hStdError = output;
        startup.lpAttributeList = attributes;
        std::wstring command = quote_argument(exe);
        for (const auto &arg : args) command += L" " + quote_argument(arg);
        PROCESS_INFORMATION process{};
        bool created = configured && CreateProcessW(exe.c_str(), command.data(), nullptr, nullptr, TRUE,
            EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED | CREATE_NO_WINDOW, nullptr,
            cwd.c_str(), &startup.StartupInfo, &process);
        if (initialized) DeleteProcThreadAttributeList(attributes);
        CloseHandle(output); CloseHandle(input);
        if (!created) { stop(); throw std::runtime_error("Cannot start the selected OpenSCAD executable."); }
        // Assignment precedes ResumeThread: children cannot escape before ownership is established.
        if (!AssignProcessToJobObject(m_job, process.hProcess) || ResumeThread(process.hThread) == DWORD(-1)) {
            TerminateProcess(process.hProcess, 1);
            CloseHandle(process.hThread); CloseHandle(process.hProcess); stop();
            throw std::runtime_error("Cannot attach renderer to its private job.");
        }
        CloseHandle(process.hThread);
        m_process = process.hProcess;
    }
};
}
#endif
