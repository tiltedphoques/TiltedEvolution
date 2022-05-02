
#include <strsafe.h>

#include <MinHook.h>
#include <winternl.h>

#include <FunctionHook.hpp>
#include <TiltedCore/Initializer.hpp>

#include "DllBlocklist.h"
#include "TargetConfig.h"
#include "Utils/NtInternal.h"
#include "launcher.h"

namespace
{
std::filesystem::path s_OverridePath;

DWORD(WINAPI* RealGetModuleFileNameW)(HMODULE, LPWSTR, DWORD) = nullptr;
DWORD(WINAPI* RealGetModuleFileNameA)(HMODULE, LPSTR, DWORD) = nullptr;
HMODULE(WINAPI* RealGetModuleHandleW)(LPCWSTR) = nullptr;
HMODULE(WINAPI* RealGetModuleHandleA)(LPSTR) = nullptr;
NTSTATUS(WINAPI* RealLdrLoadDll)(const wchar_t*, uint32_t*, UNICODE_STRING*, HANDLE*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllHandle)(PWSTR, PULONG, PUNICODE_STRING, PVOID*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllFullName)(HMODULE, PUNICODE_STRING) = nullptr;

bool IsUsingMO2()
{
    return GetModuleHandleW(L"usvfs_x64.dll");
}

// if the module path functions are called from any TP dll within the TP bin folder we return the real path to
// TiltedOnline.exe
bool IsLocalModulePath(void* apAddress)
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

NTSTATUS WINAPI TP_LdrGetDllHandle(PWSTR DllPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* DllHandle)
{
    TP_EMPTY_HOOK_PLACEHOLDER;
    if (std::wcsncmp(TARGET_NAME L".exe", DllName->Buffer, DllName->Length) == 0 && launcher::GetLaunchContext())
    {
        *DllHandle = NtInternal::ThePeb()->pImageBase;
        return 0; // success
    }

    return RealLdrGetDllHandle(DllPath, DllCharacteristics, DllName, DllHandle);
}

struct TlsState
{
    bool inLoadDll;
};
static DWORD g_tlsHandle;

static TlsState* GetTls()
{
    auto data = TlsGetValue(g_tlsHandle);

    if (!data)
    {
        data = HeapAlloc(GetProcessHeap(), 0, sizeof(TlsState));
        memset(data, 0, sizeof(TlsState));

        TlsSetValue(g_tlsHandle, data);
    }

    return reinterpret_cast<TlsState*>(data);
}

NTSTATUS WINAPI TP_LdrGetDllFullName(HMODULE Module, PUNICODE_STRING DllName)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    //if (!Module)
    //    Module = NtInternal::ThePeb()->pImageBase;

    // need to detect if called from skyrimtogether.exe (e.g our own code), or game code.

    //if (!GetTls()->inLoadDll)
    {
        if (Module == nullptr && launcher::GetLaunchContext())
        {
            auto& aExePath = launcher::GetLaunchContext()->exePath;
            RtlInitUnicodeString(DllName, aExePath.c_str());
            return 0; // sucess
        }
    }
 
    return RealLdrGetDllFullName(Module, DllName);
}

static DWORD WINAPI TP_GetModuleFileNameW(HMODULE aModule, LPWSTR alpFilename, DWORD aSize)
{
    // trampoline space for USVFS
    TP_EMPTY_HOOK_PLACEHOLDER;

    if (aModule == nullptr || aModule == NtInternal::ThePeb()->pImageBase)
    {
        void* rbp = _ReturnAddress();
        if (!IsLocalModulePath(rbp) && launcher::GetLaunchContext())
        {
            auto& aExePath = launcher::GetLaunchContext()->exePath;
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

        if (!IsLocalModulePath(rbp) && launcher::GetLaunchContext())
        {
            auto aExePath = launcher::GetLaunchContext()->exePath.string();
            StringCchCopyA(alpFileName, aSize, aExePath.c_str());

            return static_cast<DWORD>(std::strlen(alpFileName));
        }
    }

    return RealGetModuleFileNameA(aModule, alpFileName, aSize);
}

NTSTATUS WINAPI TP_LdrLoadDll(const wchar_t* apPath, uint32_t* apFlags, UNICODE_STRING* apFileName, HANDLE* apHandle)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    GetTls()->inLoadDll = true;

    std::wstring_view fileName(apFileName->Buffer, apFileName->Length / sizeof(wchar_t));
    size_t pos = fileName.find_last_of(L'\\');
    // just to make sure..
    if (pos != std::wstring_view::npos && (pos + 1) != fileName.length())
    {
        if (stubs::IsDllBlocked(&fileName[pos + 1]))
        {
            // invalid image hash
            // this signals windows to *NOT TRY* loading it again at a later time.
            return 0xC0000428;
        }
    }

    // will be used in the future to blacklist mods that break ST/FT
    auto result = RealLdrLoadDll(apPath, apFlags, apFileName, apHandle);

    GetTls()->inLoadDll = false;
    return result;
}
} // namespace

// free function until we introduce a second TP initializer init chain
void CoreStubsInit()
{
    s_OverridePath = TiltedPhoques::GetPath();

    MH_Initialize();

#if 1
    // check if we are started from mod organizer 2, if so, we disable our hooks.
    // this will require a proper fix in the future, but for now it does the job.
    if (!IsUsingMO2())
    {
        MH_CreateHookApi(L"Kernel32.dll", "GetModuleFileNameW", &TP_GetModuleFileNameW,
                         (void**)&RealGetModuleFileNameW);
        MH_CreateHookApi(L"Kernel32.dll", "GetModuleFileNameA", &TP_GetModuleFileNameA,
                         (void**)&RealGetModuleFileNameA);
    }
#endif

    // SKSE calls https://github.com/ianpatt/skse64/blob/d79e8f081194f538c24d493e1b57331d837a25c0/skse64_common/Utilities.cpp#L11

    MH_CreateHookApi(L"ntdll.dll", "LdrGetDllHandle", &TP_LdrGetDllHandle, (void**)&RealLdrGetDllHandle);
    //MH_CreateHookApi(L"ntdll.dll", "LdrGetDllFullName", &TP_LdrGetDllFullName, (void**)&RealLdrGetDllFullName);
    MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", &TP_LdrLoadDll, (void**)&RealLdrLoadDll);
    MH_EnableHook(nullptr);
}
