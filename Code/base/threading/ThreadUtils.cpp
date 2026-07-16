#include "ThreadUtils.h"

#if defined(_WIN32)
#include <Windows.h>
#include <string>
#include <vector>

using TSetThreadDescription = HRESULT(WINAPI*)(HANDLE, PCWSTR);
#else
#include <cstring>
#include <pthread.h>
// Linux thread name limit is 16 bytes (15 chars + null terminator)
static constexpr size_t LINUX_THREAD_NAME_MAX = 16;
#endif

namespace Base
{

#if defined(_WIN32)

static std::wstring ToWString(const char* src)
{
    if (!src || !*src)
        return L"";

    size_t srcLen = std::strlen(src);
    if (srcLen > 256)
        return L""; // Limit to reasonable size

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
    if (size_needed <= 0)
        return L"";

    std::vector<wchar_t> buffer(size_needed);
    MultiByteToWideChar(CP_UTF8, 0, src, -1, buffer.data(), size_needed);
    return std::wstring(buffer.data());
}

static bool SetThreadNameWindows(HANDLE hThread, const char* name)
{
    // We dynamically load SetThreadDescription to ensure the application
    // doesn't crash on older Windows versions (pre-Win10 1607) where this
    // export is missing from Kernel32.dll.
    static const auto pSetThreadDescription = reinterpret_cast<TSetThreadDescription>(
        reinterpret_cast<void*>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetThreadDescription"))
    );

    if (pSetThreadDescription)
    {
        std::wstring wName = ToWString(name);
        HRESULT hr = pSetThreadDescription(hThread, wName.c_str());
        return SUCCEEDED(hr);
    }

    return false;
}

bool SetThreadName(const ThreadHandle& acHandle, const char* apThreadName)
{
    if (!acHandle || !apThreadName) return false;
    return SetThreadNameWindows(acHandle, apThreadName);
}

bool SetCurrentThreadName(const char* apThreadName)
{
    if (!apThreadName) return false;
    return SetThreadNameWindows(::GetCurrentThread(), apThreadName);
}

#else

static bool SetThreadNameLinux(pthread_t thread, const char* name)
{
    if (!name) return false;

    // Linux limits thread names to 16 chars (including \0). We strictly truncate to fit the buffer
    char buf[LINUX_THREAD_NAME_MAX] = {};
    std::strncpy(buf, name, LINUX_THREAD_NAME_MAX - 1);
    buf[LINUX_THREAD_NAME_MAX - 1] = '\0';

    return pthread_setname_np(thread, buf) == 0;
}

bool SetThreadName(const ThreadHandle& acHandle, const char* apThreadName)
{
    return SetThreadNameLinux(acHandle, apThreadName);
}

bool SetCurrentThreadName(const char* apThreadName)
{
    return SetThreadNameLinux(pthread_self(), apThreadName);
}

#endif

} // namespace Base
