//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//

#pragma once

class LogWriter final
{
public:
    LogWriter()
    {
        InitializeSRWLock(&m_stdoutLock);

        DWORD dwMode;
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        if (!GetConsoleMode(hConsole, &dwMode)) {
            m_isConsole = false;
        }
        else {
            m_isConsole = true;
        }

        _setmode(_fileno(stdout), _O_U8TEXT);
    };

    ~LogWriter() {}

private:
    SRWLOCK m_stdoutLock;
    bool m_isConsole;
    HANDLE hConsole;

    void FlushStdOut()
    {
        if (m_isConsole)
        {
            fflush(stdout);
        }
    }

public :
    bool WriteLog(
        _In_ HANDLE       FileHandle,
        _In_ LPCVOID      Buffer,
        _In_ DWORD        NumberOfBytesToWrite,
        _In_ LPDWORD      NumberOfBytesWritten,
        _In_ LPOVERLAPPED Overlapped
    )
    {
        bool result;

        AcquireSRWLockExclusive(&m_stdoutLock);

        result = WriteFile(FileHandle, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten, Overlapped);

        ReleaseSRWLockExclusive(&m_stdoutLock);

        return result;
    }

    void WriteConsoleLog(
        _In_ const std::wstring&& LogMessage
    )
    {
        AcquireSRWLockExclusive(&m_stdoutLock);

        std::wstring output = LogMessage + L"\n";

        if (m_isConsole) {
            WriteConsoleW(hConsole, output.c_str(), wcslen(output.c_str()), NULL, NULL);
        } else {
            wprintf(output.c_str());
        }

        FlushStdOut();

        ReleaseSRWLockExclusive(&m_stdoutLock);
    }

    void WriteConsoleLog(
        _In_ const std::wstring& LogMessage
    )
    {
        AcquireSRWLockExclusive(&m_stdoutLock);

        std::wstring output = LogMessage + L"\n";

        if (m_isConsole) {
            WriteConsoleW(hConsole, output.c_str(), wcslen(output.c_str()), NULL, NULL);
        } else {
            wprintf(output.c_str());
        }

        FlushStdOut();

        ReleaseSRWLockExclusive(&m_stdoutLock);
    }

    void TraceError(
        _In_ LPCWSTR Message
    )
    {
        SYSTEMTIME st;
        GetSystemTime(&st);

        std::wstring formattedMessage = Utility::FormatString(L"[%s][LOGMONITOR] ERROR: %s",
            Utility::SystemTimeToString(st).c_str(),
            Message);

        WriteConsoleLog(formattedMessage);
    }

    void TraceWarning(
        _In_ LPCWSTR Message
    )
    {
        SYSTEMTIME st;
        GetSystemTime(&st);

        std::wstring formattedMessage = Utility::FormatString(L"[%s][LOGMONITOR] WARNING: %s",
            Utility::SystemTimeToString(st).c_str(),
            Message);

        WriteConsoleLog(formattedMessage);
    }

    void TraceInfo(
        _In_ LPCWSTR Message
    )
    {
        SYSTEMTIME st;
        GetSystemTime(&st);

        std::wstring formattedMessage = Utility::FormatString(L"[%s][LOGMONITOR] INFO: %s",
            Utility::SystemTimeToString(st).c_str(),
            Message);

        WriteConsoleLog(formattedMessage);
    }
};

extern LogWriter logWriter;
