
#include <strsafe.h>

#include <MinHook.h>
#include <winternl.h>

#include <FunctionHook.hpp>
#include <TiltedCore/Initializer.hpp>

#include "Launcher.h"
#include "DllBlocklist.h"
#include "Utils/NtInternal.h"

static fs::path s_OverridePath;

static DWORD(WINAPI* RealGetModuleFileNameW)(HMODULE, LPWSTR, DWORD) = nullptr;
static DWORD(WINAPI* RealGetModuleFileNameA)(HMODULE, LPSTR, DWORD) = nullptr;
static HMODULE(WINAPI* RealGetModuleHandleW)(LPCWSTR) = nullptr;
static HMODULE(WINAPI* RealGetModuleHandleA)(LPSTR) = nullptr;
static NTSTATUS(WINAPI* RealLdrLoadDll)(const wchar_t*, uint32_t*, UNICODE_STRING*, HANDLE*) = nullptr;

// if the module path functions are called from any TP dll within the TP bin folder we return the real path to TiltedOnline.exe
bool IsLocalModulePath(void *apAddress)
{
    HMODULE pModule = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(apAddress), &pModule);

    if (pModule == nullptr)
        return false;

    std::wstring buf;

    // safe way of getting module file name
    size_t i = 1;
    DWORD res = 0;
    do 
    {
        buf.resize(i * MAX_PATH);
        res = RealGetModuleFileNameW(pModule, buf.data(), static_cast<DWORD>(buf.length()));
        i++;
    } while (res == ERROR_INSUFFICIENT_BUFFER);

    return buf.find(s_OverridePath.native()) != std::wstring::npos;
}

static DWORD WINAPI TP_GetModuleFileNameW(HMODULE aModule, LPWSTR alpFilename, DWORD aSize)
{
    // trampoline space for USVFS
    TP_EMPTY_HOOK_PLACEHOLDER;

    if (aModule == nullptr || aModule == NtInternal::ThePeb()->pImageBase)
    {
        void* rbp = _ReturnAddress(); 
        if (!IsLocalModulePath(rbp) && GetLauncher())
        {
            auto& aExePath = GetLauncher()->GetExePath();
            StringCchCopyW(alpFilename, aSize, aExePath.c_str());

            return static_cast<DWORD>(std::wcslen(alpFilename));
        }
    }

    return RealGetModuleFileNameW(aModule, alpFilename, aSize);
}

static DWORD WINAPI TP_GetModuleFileNameA(HMODULE aModule, LPSTR alpFileName, DWORD aSize)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    if (aModule == nullptr || aModule == NtInternal::ThePeb()->pImageBase)
    {
        void* rbp = _ReturnAddress();

        if (!IsLocalModulePath(rbp) && GetLauncher())
        {

            auto aExePath = GetLauncher()->GetExePath().u8string();
            StringCchCopyA(alpFileName, aSize, aExePath.c_str());

            return static_cast<DWORD>(std::strlen(alpFileName));
        }
    }

    return RealGetModuleFileNameA(aModule, alpFileName, aSize);
}

HMODULE WINAPI TP_GetModuleHandleW(LPCWSTR alpModuleName)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    constexpr wchar_t* kMapList[] = { 
        L"SkyrimSE.exe", 
        L"SkyrimVR.exe", 
        L"Fallout4.exe", 
        L"Fallout4VR.exe"
    };

    if (alpModuleName)
    {
        for (const auto* it : kMapList)
        {
            if (std::wcscmp(it, alpModuleName) == 0)
            {
                alpModuleName = nullptr;
                break;
            }
        }
    }

    return RealGetModuleHandleW(alpModuleName);
}

HMODULE WINAPI TP_GetModuleHandleA(LPSTR alpModuleName)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    constexpr char* kMapList[] = {
        "SkyrimSE.exe", 
        "SkyrimVR.exe", 
        "Fallout4.exe", 
        "Fallout4VR.exe"
    };

    if (alpModuleName)
    {
        for (const auto* it : kMapList)
        {
            if (std::strcmp(it, alpModuleName) == 0)
            {
                alpModuleName = nullptr;
                break;
            }
        }
    }

    return RealGetModuleHandleA(alpModuleName);
}

NTSTATUS WINAPI TP_LdrLoadDll(const wchar_t* apPath, uint32_t* apFlags, UNICODE_STRING* apFileName, HANDLE* apHandle)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    std::wstring fileName(apFileName->Buffer, apFileName->Length / sizeof(wchar_t));
    size_t pos = fileName.find_last_of(L'\\');
    // just to make sure..
    if (pos != std::wstring::npos && (pos + 1) != fileName.length())
    {
        if (stubs::IsDllBlocked(&fileName[pos + 1]))
        {
            return STATUS_DLL_NOT_FOUND;
        }
    }

    // will be used in the future to blacklist mods that break ST/FT
    return RealLdrLoadDll(apPath, apFlags, apFileName, apHandle);
}

// free function until we introduce a second TP initializer init chain
void CoreStubsInit()
{
    s_OverridePath = TiltedPhoques::GetPath();

    MH_Initialize();

    // check if we are started from mod organizer 2, if so, we disable our hooks.
    // this will require a proper fix in the future, but for now it does the job.
    if (!GetModuleHandleW(L"usvfs_x64.dll"))
    {
        MH_CreateHookApi(L"Kernel32.dll", "GetModuleFileNameW", &TP_GetModuleFileNameW,
                         (void**)&RealGetModuleFileNameW);
        MH_CreateHookApi(L"Kernel32.dll", "GetModuleFileNameA", &TP_GetModuleFileNameA,
                         (void**)&RealGetModuleFileNameA);
        MH_CreateHookApi(L"Kernel32.dll", "GetModuleHandleW", &TP_GetModuleHandleW, (void**)&RealGetModuleHandleW);
        MH_CreateHookApi(L"Kernel32.dll", "GetModuleHandleA", &TP_GetModuleHandleA, (void**)&RealGetModuleHandleA);
    }

    MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", &TP_LdrLoadDll, (void**)&RealLdrLoadDll);
    MH_EnableHook(nullptr);
}
