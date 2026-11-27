// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#include <windows.h>
#include <shellapi.h>
#include <wchar.h>

#define SPECTRA_PATH_CAPACITY 32768
#define SPECTRA_COMMAND_CAPACITY 65535

static BOOL append_text(WCHAR* output, DWORD capacity, const WCHAR* text)
{
    const size_t used = wcslen(output);
    const size_t needed = wcslen(text);
    if (used + needed + 1 > capacity) {
        return FALSE;
    }
    {
        size_t index;
        for (index = 0; index <= needed; ++index) {
            output[used + index] = text[index];
        }
    }
    return TRUE;
}

static BOOL append_character(WCHAR* output, DWORD capacity, WCHAR character)
{
    const size_t used = wcslen(output);
    if (used + 2 > capacity) {
        return FALSE;
    }
    output[used] = character;
    output[used + 1] = L'\0';
    return TRUE;
}

static BOOL append_quoted(WCHAR* output, DWORD capacity, const WCHAR* value)
{
    const WCHAR* cursor = value;
    BOOL needsQuotes = *value == L'\0';
    while (*cursor != L'\0') {
        if (*cursor == L' ' || *cursor == L'\t' || *cursor == L'\n' || *cursor == L'\v' || *cursor == L'"') {
            needsQuotes = TRUE;
            break;
        }
        ++cursor;
    }
    if (!needsQuotes) {
        return append_text(output, capacity, value);
    }

    size_t backslashes = 0;
    if (!append_character(output, capacity, L'"')) {
        return FALSE;
    }
    for (cursor = value; *cursor != L'\0'; ++cursor) {
        if (*cursor == L'\\') {
            ++backslashes;
            continue;
        }
        if (*cursor == L'"') {
            size_t index;
            for (index = 0; index < backslashes * 2 + 1; ++index) {
                if (!append_character(output, capacity, L'\\')) {
                    return FALSE;
                }
            }
            if (!append_character(output, capacity, L'"')) {
                return FALSE;
            }
            backslashes = 0;
            continue;
        }
        while (backslashes > 0) {
            if (!append_character(output, capacity, L'\\')) {
                return FALSE;
            }
            --backslashes;
        }
        if (!append_character(output, capacity, *cursor)) {
            return FALSE;
        }
    }
    while (backslashes > 0) {
        if (!append_character(output, capacity, L'\\')) {
            return FALSE;
        }
        --backslashes;
    }
    return append_character(output, capacity, L'"');
}

static BOOL file_exists(const WCHAR* path)
{
    const DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

static BOOL has_directory_argument(int argumentCount, WCHAR** arguments)
{
    int index;
    for (index = 1; index < argumentCount; ++index) {
        const WCHAR* argument = arguments[index];
        if (wcscmp(argument, L"--dir") == 0 || wcscmp(argument, L"-d") == 0 || wcsncmp(argument, L"--dir=", 6) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL join_path(WCHAR* output, DWORD capacity, const WCHAR* root, const WCHAR* relative)
{
    output[0] = L'\0';
    return append_text(output, capacity, root) && append_text(output, capacity, relative);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLineAscii, int showCommand)
{
    WCHAR modulePath[SPECTRA_PATH_CAPACITY];
    WCHAR root[SPECTRA_PATH_CAPACITY];
    WCHAR runtime[SPECTRA_PATH_CAPACITY];
    WCHAR executable[SPECTRA_PATH_CAPACITY];
    WCHAR marker[SPECTRA_PATH_CAPACITY];
    WCHAR commandLine[SPECTRA_COMMAND_CAPACITY];
    STARTUPINFOW startup = { 0 };
    PROCESS_INFORMATION process = { 0 };
    WCHAR** arguments;
    int argumentCount = 0;
    BOOL portable;
    BOOL hasDirectory;
    DWORD length;
    DWORD index;
    DWORD exitCode = 1;

    (void)instance;
    (void)previousInstance;
    (void)commandLineAscii;
    (void)showCommand;

    length = GetModuleFileNameW(NULL, modulePath, SPECTRA_PATH_CAPACITY);
    if (length == 0 || length >= SPECTRA_PATH_CAPACITY) {
        MessageBoxW(NULL, L"无法确定光谱启动器目录。", L"光谱", MB_OK | MB_ICONERROR);
        return 2;
    }

    for (index = length; index > 0; --index) {
        if (modulePath[index - 1] == L'\\' || modulePath[index - 1] == L'/') {
            modulePath[index - 1] = L'\0';
            break;
        }
    }
    if (index == 0 || !append_text(root, SPECTRA_PATH_CAPACITY, modulePath)) {
        MessageBoxW(NULL, L"无法确定光谱启动器目录。", L"光谱", MB_OK | MB_ICONERROR);
        return 2;
    }
    if (!join_path(runtime, SPECTRA_PATH_CAPACITY, root, L"\\.Spectra\\runtime") ||
        !join_path(executable, SPECTRA_PATH_CAPACITY, runtime, L"\\spectra.exe") ||
        !join_path(marker, SPECTRA_PATH_CAPACITY, runtime, L"\\portable.mode")) {
        MessageBoxW(NULL, L"启动器路径过长。", L"光谱", MB_OK | MB_ICONERROR);
        return 2;
    }
    if (!file_exists(executable)) {
        MessageBoxW(NULL, L"启动器运行文件缺失：.Spectra\\runtime\\spectra.exe", L"光谱", MB_OK | MB_ICONERROR);
        return 3;
    }

    arguments = CommandLineToArgvW(GetCommandLineW(), &argumentCount);
    if (arguments == NULL) {
        MessageBoxW(NULL, L"无法读取启动参数。", L"光谱", MB_OK | MB_ICONERROR);
        return 4;
    }

    commandLine[0] = L'\0';
    if (!append_quoted(commandLine, SPECTRA_COMMAND_CAPACITY, executable)) {
        LocalFree(arguments);
        MessageBoxW(NULL, L"启动命令过长。", L"光谱", MB_OK | MB_ICONERROR);
        return 4;
    }
    for (index = 1; index < (DWORD)argumentCount; ++index) {
        if (!append_character(commandLine, SPECTRA_COMMAND_CAPACITY, L' ') ||
            !append_quoted(commandLine, SPECTRA_COMMAND_CAPACITY, arguments[index])) {
            LocalFree(arguments);
            MessageBoxW(NULL, L"启动命令过长。", L"光谱", MB_OK | MB_ICONERROR);
            return 4;
        }
    }

    portable = file_exists(marker);
    hasDirectory = has_directory_argument(argumentCount, arguments);
    LocalFree(arguments);

    if (portable && !hasDirectory) {
        if (!append_text(commandLine, SPECTRA_COMMAND_CAPACITY, L" --dir ") ||
            !append_quoted(commandLine, SPECTRA_COMMAND_CAPACITY, root)) {
            MessageBoxW(NULL, L"启动命令过长。", L"光谱", MB_OK | MB_ICONERROR);
            return 4;
        }
    }

    startup.cb = sizeof(startup);
    if (!CreateProcessW(executable, commandLine, NULL, NULL, FALSE, 0, NULL, runtime, &startup, &process)) {
        MessageBoxW(NULL, L"无法启动光谱主程序。请确认 .Spectra\\runtime 完整。", L"光谱", MB_OK | MB_ICONERROR);
        return 5;
    }

    WaitForSingleObject(process.hProcess, INFINITE);
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return (int)exitCode;
}
