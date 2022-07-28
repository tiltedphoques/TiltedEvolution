
#include <strsafe.h>

#include <MinHook.h>
#include <winternl.h>

#include <FunctionHook.hpp>
#include <TiltedCore/Initializer.hpp>

#include "DllBlocklist.h"
#include "TargetConfig.h"
#include "Utils/NtInternal.h"
#include "utils/Error.h"
#include "launcher.h"

// defined in MemoryLayout.cpp
extern bool IsThisExeAddress(const uint8_t* apAddress);
extern bool IsGameMemoryAddress(const uint8_t* apAddress);

extern "C" __declspec(dllimport) NTSTATUS WINAPI LdrGetDllFullName(HMODULE, PUNICODE_STRING);

namespace
{
std::wstring s_OverridePath;

DWORD(WINAPI* RealGetModuleFileNameW)(HMODULE, LPWSTR, DWORD) = nullptr;
DWORD(WINAPI* RealGetModuleFileNameA)(HMODULE, LPSTR, DWORD) = nullptr;
HMODULE(WINAPI* RealGetModuleHandleW)(LPCWSTR) = nullptr;
HMODULE(WINAPI* RealGetModuleHandleA)(LPSTR) = nullptr;
NTSTATUS(WINAPI* RealLdrLoadDll)(const wchar_t*, uint32_t*, UNICODE_STRING*, HANDLE*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllHandle)(PWSTR, PULONG, PUNICODE_STRING, PVOID*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllFullName)(HMODULE, PUNICODE_STRING) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllHandleEx)
(ULONG Flags, PWSTR DllPath, PULONG DllCharacteristics, UNICODE_STRING* DllName, PVOID* DllHandle) = nullptr;

inline bool IsUsingMO2()
{
    return GetModuleHandleW(L"usvfs_x64.dll");
}

inline bool IsMyModule(HMODULE aHmod)
{
    return aHmod == nullptr || aHmod == NtInternal::ThePeb()->pImageBase;
}

HMODULE HModFromAddress(void* apAddress)
{
    HMODULE pModule = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(apAddress), &pModule);

    return pModule;
}

DWORD MYGetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
    if (!nSize)
        return 0;

    if (nSize > 0x7FFF)
        nSize = 0x7FFF;

    UNICODE_STRING str{};
    str.Buffer = lpFilename;
    str.MaximumLength = static_cast<USHORT>(sizeof(wchar_t) * nSize - sizeof(wchar_t));

    LdrGetDllFullName(hModule, &str);

    DWORD nChars = str.Length / sizeof(wchar_t);
    str.Buffer[nChars] = 0;
    return nChars;
}

bool IsLocalModulePath(HMODULE aHmod)
{
    std::wstring buf;

    size_t i = 1;
    DWORD res = 0;
    do
    {
        buf.resize(i * MAX_PATH);
        res = MYGetModuleFileNameW(aHmod, buf.data(), static_cast<DWORD>(buf.length()));
        i++;
    } while (res == ERROR_INSUFFICIENT_BUFFER);

    // does the file exist in the ST dir?
    return buf.find(s_OverridePath) != std::wstring::npos;
}

// some mods do GetModuleHandle("SkyrimSE.exe") for some reason instead of GetModuleHandle(nullptr)
NTSTATUS WINAPI TP_LdrGetDllHandle(PWSTR DllPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* DllHandle)
{
    // no need to check for nullptr here, this is handeled by the higher level GetModuleHandle function.
    TP_EMPTY_HOOK_PLACEHOLDER;

    if (std::wcsncmp(TARGET_NAME L".exe", DllName->Buffer, DllName->Length) == 0)
    {
        *DllHandle = NtInternal::ThePeb()->pImageBase;
        return 0; // success
    }

    return RealLdrGetDllHandle(DllPath, DllCharacteristics, DllName, DllHandle);
}

NTSTATUS WINAPI TP_LdrGetDllHandleEx(ULONG Flags, PWSTR DllPath, PULONG DllCharacteristics, UNICODE_STRING* DllName, PVOID *DllHandle) 
{
    if (DllName && std::wcsncmp(TARGET_NAME L".exe", DllName->Buffer, DllName->Length) == 0)
    {
        *DllHandle = NtInternal::ThePeb()->pImageBase;
        return 0; // success
    }

    return RealLdrGetDllHandleEx(Flags, DllPath, DllCharacteristics, DllName, DllHandle);
}

NTSTATUS WINAPI TP_LdrGetDllFullName(HMODULE Module, PUNICODE_STRING DllName)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    void* rsp = _ReturnAddress();

    // we need to detect if GetModuleFileName(null) was called from our own code or the game code/other modules
    // if (!GetTls()->inLoadDll)
    if (launcher::GetLaunchContext())
    {
        if (Module == nullptr || Module == NtInternal::ThePeb()->pImageBase)
        {
            auto& aExePath = launcher::GetLaunchContext()->exePath;
            RtlInitUnicodeString(DllName, aExePath.c_str());
            return 0; // sucess
        }
    }

    return RealLdrGetDllFullName(Module, DllName);
}

bool NeedsToFool(void* pRbp, bool* wantsTruth = nullptr)
{
    // game code/stub segment within this exe needs to be fooled
    if (IsThisExeAddress(static_cast<uint8_t*>(pRbp)))
    {
        return IsGameMemoryAddress(static_cast<uint8_t*>(pRbp));
    }

    // this heuristic indicates hooked game code... that is still owned by us...
    // not recognized immedeatly, but still looks like game code...
    HMODULE hMod = HModFromAddress(pRbp);

    // simple debug hook
#if 0
    if (hMod == GetModuleHandleW(L"NvCameraAllowlisting64.dll"))
    {
        __debugbreak();
    }
#endif

    if (hMod == NtInternal::ThePeb()->pImageBase || 
        hMod == nullptr /*This is a hook, virtual allocd, not owned by anybody, so we assign ownership to the ST directory*/)
    {
        if (wantsTruth)
            *wantsTruth = true;
        return false;
    }

    return !IsLocalModulePath(hMod);
}

DWORD WINAPI TP_GetModuleFileNameW(HMODULE aModule, LPWSTR alpFilename, DWORD aSize)
{
    // trampoline space for USVFS
    TP_EMPTY_HOOK_PLACEHOLDER;

    void* rbp = _ReturnAddress();
    // PrintOwnerNa me(rbp);

    bool force = false;
    if (IsMyModule(aModule) && NeedsToFool(rbp, &force) && launcher::GetLaunchContext())
    {
        auto& aExePath = launcher::GetLaunchContext()->exePath;
        StringCchCopyW(alpFilename, aSize, aExePath.c_str());

        return static_cast<DWORD>(std::wcslen(alpFilename));
    }

    if (force)
        return MYGetModuleFileNameW(aModule, alpFilename, aSize);

    return RealGetModuleFileNameW(aModule, alpFilename, aSize);
}

struct ScopedOSHeapItem
{
    ScopedOSHeapItem(size_t aSize)
    {
        m_pBlock = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, aSize);
    }

    ~ScopedOSHeapItem()
    {
        if (m_pBlock)
            HeapFree(GetProcessHeap(), 0, m_pBlock);
    }

    void* m_pBlock;
};

// NOTE(Vince): Introduce one layer of indirection by calling GetModuleFileNameW function directly, to trigger usvfs
// indirection.
DWORD WINAPI TP_GetModuleFileNameA(HMODULE aModule, char* alpFileName, DWORD aBufferSize)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    void* rbp = _ReturnAddress();
    if (IsMyModule(aModule) && NeedsToFool(rbp) && launcher::GetLaunchContext())
    {
        auto aExePath = launcher::GetLaunchContext()->exePath.string();
        StringCchCopyA(alpFileName, aBufferSize, aExePath.c_str());

        return static_cast<DWORD>(std::strlen(alpFileName));
    }

    ScopedOSHeapItem wideBuffer((aBufferSize * sizeof(wchar_t)) + 1);

    wchar_t* pBuffer = static_cast<wchar_t*>(wideBuffer.m_pBlock);
    DWORD result = RealGetModuleFileNameW(aModule, pBuffer, aBufferSize * sizeof(wchar_t));
    if (result == 0)
    {
        return result;
    }

    UNICODE_STRING source{};
    RtlInitUnicodeString(&source, pBuffer);

    // convert using the proper OS function
    ANSI_STRING dest{.Length = static_cast<USHORT>(result),
                     .MaximumLength = static_cast<USHORT>(aBufferSize),
                     .Buffer = alpFileName};
    if (RtlUnicodeStringToAnsiString(&dest, &source, FALSE) != 0)
        return 0;

    return result;
}

// we use this function to enforce the DLL load policy
NTSTATUS WINAPI TP_LdrLoadDll(const wchar_t* apPath, uint32_t* apFlags, UNICODE_STRING* apFileName, HANDLE* apHandle)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    std::wstring_view fileName(apFileName->Buffer, apFileName->Length / sizeof(wchar_t));
    size_t pos = fileName.find_last_of(L'\\');
    if (pos != std::wstring_view::npos && (pos + 1) != fileName.length())
    {
        if (stubs::IsDllBlocked(&fileName[pos + 1]))
        {
            // invalid image hash
            // this signals windows to *NOT TRY* loading it again at a later time.
            return 0xC0000428;
        }
    }

    return RealLdrLoadDll(apPath, apFlags, apFileName, apHandle);
}
} // namespace

#define VALIDATE(x)                                                                                                    \
    if (x != MH_OK)                                                                                                    \
        Die(L"CoreStubsInit(): Fatal Minhook error.", true);

// pre eat hook?? loadmodule hook??
// the idea would be to link against an external dll, which in its init routine then hooks, so we execute before mo2?
void CoreStubsInit()
{
    s_OverridePath = TiltedPhoques::GetPath().wstring();
    std::replace(s_OverridePath.begin(), s_OverridePath.end(), L'/', L'\\');

    VALIDATE(MH_Initialize());

    if (!IsUsingMO2())
    {
        // we need two hooks here, even if this is kinda redundant, since we want to capture the RBP register
        // to detect if its game code, or launcher code being executed
        VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleFileNameW", &TP_GetModuleFileNameW,
                                  (void**)&RealGetModuleFileNameW));
        VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleFileNameA", &TP_GetModuleFileNameA,
                                  (void**)&RealGetModuleFileNameA));
    }
    else
    {
        // https://github.com/ModOrganizer2/usvfs/blob/master/src/usvfs_dll/hooks/kernel32.h#L42
        VALIDATE(MH_CreateHookApi(L"usvfs_x64.dll", "?hook_GetModuleFileNameW@usvfs@@YAKPEAUHINSTANCE__@@PEA_WK@Z",
                                  &TP_GetModuleFileNameW, (void**)&RealGetModuleFileNameW));
        VALIDATE(MH_CreateHookApi(L"KernelBase.dll", "GetModuleFileNameA", &TP_GetModuleFileNameA,
                                  (void**)&RealGetModuleFileNameA));
    }

    // SKSE calls
    // https://github.com/ianpatt/skse64/blob/d79e8f081194f538c24d493e1b57331d837a25c0/skse64_common/Utilities.cpp#L11

    //VALIDATE(MH_CreateHookApi(L"ntdll.dll", "LdrGetDllHandle", &TP_LdrGetDllHandle, (void**)&RealLdrGetDllHandle));
    VALIDATE(MH_CreateHookApi(L"ntdll.dll", "LdrGetDllHandleEx", &TP_LdrGetDllHandleEx, (void**)&RealLdrGetDllHandleEx));

    // TODO(Vince): we need some check if usvfs already fucked with this?
    // MH_CreateHookApi(L"ntdll.dll", "LdrGetDllFullName", &TP_LdrGetDllFullName, (void**)&RealLdrGetDllFullName);
    VALIDATE(MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", &TP_LdrLoadDll, (void**)&RealLdrLoadDll));
    VALIDATE(MH_EnableHook(nullptr));
}

#undef VALIDATE
