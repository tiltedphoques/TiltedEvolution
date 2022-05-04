
#include <strsafe.h>

#include <MinHook.h>
#include <winternl.h>

#include <FunctionHook.hpp>
#include <TiltedCore/Initializer.hpp>

#include "DllBlocklist.h"
#include "TargetConfig.h"
#include "Utils/NtInternal.h"
#include "launcher.h"

// owned means its code executing within ST exe, that needs to behave like
extern bool IsThisExeAddress(uint8_t* apAddress);
extern bool IsGameMemoryAddress(uint8_t* apAddress);

namespace
{
std::wstring s_OverridePath;

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

DWORD(WINAPI* RealGetModuleFileNameW)(HMODULE, LPWSTR, DWORD) = nullptr;
DWORD(WINAPI* RealGetModuleFileNameA)(HMODULE, LPSTR, DWORD) = nullptr;
HMODULE(WINAPI* RealGetModuleHandleW)(LPCWSTR) = nullptr;
HMODULE(WINAPI* RealGetModuleHandleA)(LPSTR) = nullptr;
NTSTATUS(WINAPI* RealLdrLoadDll)(const wchar_t*, uint32_t*, UNICODE_STRING*, HANDLE*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllHandle)(PWSTR, PULONG, PUNICODE_STRING, PVOID*) = nullptr;
NTSTATUS(WINAPI* RealLdrGetDllFullName)(HMODULE, PUNICODE_STRING) = nullptr;

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

extern "C" __declspec(dllimport) NTSTATUS WINAPI LdrGetDllFullName(HMODULE, PUNICODE_STRING);

DWORD MYGetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
    DWORD v3;             // edi
    NTSTATUS DllFullName; // eax
    NTSTATUS v5;          // esi
    __int64 v6;           // rbx
    NTSTATUS v8;          // ecx
    UNICODE_STRING v9{};  // [rsp+20h] [rbp-18h] BYREF

    v3 = nSize;
    if (((unsigned __int8)hModule & 3) != 0)
    {
        v8 = 0xC0000135;
    LABEL_8:
        SetLastError(v8);
        return 0;
    }
    if (nSize > 0x7FFF)
    {
        v3 = 0x7FFF;
        goto LABEL_4;
    }
    if (!nSize)
    {
        v8 = -1073741789;
        goto LABEL_8;
    }
LABEL_4:
    v9.Buffer = lpFilename;
    v9.MaximumLength = 2 * v3 - 2;
    DllFullName = LdrGetDllFullName(hModule, &v9);
    v5 = DllFullName;
    v6 = v9.Length >> 1;
    v9.Buffer[v6] = 0;
    return v6;
}

bool IsLocalModulePath(HMODULE hMod)
{
    std::wstring buf;

#if 1
    // safe way of getting module file name
    size_t i = 1;
    DWORD res = 0;
    do
    {
        buf.resize(i * MAX_PATH);
        res = MYGetModuleFileNameW(hMod, buf.data(), static_cast<DWORD>(buf.length()));
        i++;
    } while (res == ERROR_INSUFFICIENT_BUFFER);
#endif

    // game?
    return buf.find(s_OverridePath) != std::wstring::npos;
}

NTSTATUS WINAPI TP_LdrGetDllHandle(PWSTR DllPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* DllHandle)
{
    // no need to check for nullptr here, this is handeled by the higher level GetModuleHandle function.
    TP_EMPTY_HOOK_PLACEHOLDER;

    // mods that are based do GetModuleHandle("SkyrimSE.exe") for some reason instead of the much more sensible
    // nullptr
    if (std::wcsncmp(TARGET_NAME L".exe", DllName->Buffer, DllName->Length) == 0)
    {
        *DllHandle = NtInternal::ThePeb()->pImageBase;
        return 0; // success
    }

    return RealLdrGetDllHandle(DllPath, DllCharacteristics, DllName, DllHandle);
}

void PrintOwnerName(void* rbp)
{
    auto hMod = HModFromAddress(rbp);

    wchar_t buf[1024]{};
    MYGetModuleFileNameW(hMod, buf, 1024);
    OutputDebugStringW(buf);
}

NTSTATUS WINAPI TP_LdrGetDllFullName(HMODULE Module, PUNICODE_STRING DllName)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    void* rsp = _ReturnAddress();

    // if (!Module)
    //     Module = NtInternal::ThePeb()->pImageBase;

    // need to detect if called from skyrimtogether.exe (e.g our own code), or game code.

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

// TODO(Vince): this should be policy based, so we can whitelist other modules to "see" us
// e.g
// HModFromAddress == GetWhiteListedModuleHandle...

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
    if (hMod == NtInternal::ThePeb()->pImageBase || 
        hMod == nullptr /*This is a hook, virtual allocd, not owned by anybody, so we assign ownership to the ST directory*/)
    {
        if (wantsTruth)
            *wantsTruth = true;

        return false;
    }

    if (hMod == NtInternal::ThePeb()->pImageBase)
        __debugbreak();

    return IsLocalModulePath(hMod);
}

// org
// return IsGameMomoryAddress ||
//       HModFromAddress(pRbp) != NtInternal::ThePeb()->pImageBase

static DWORD WINAPI TP_GetModuleFileNameW(HMODULE aModule, LPWSTR alpFilename, DWORD aSize)
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
static DWORD WINAPI TP_GetModuleFileNameA(HMODULE aModule, char* alpFileName, DWORD aBufferSize)
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
        __debugbreak();
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

    GetTls()->inLoadDll = true;

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

    auto result = RealLdrLoadDll(apPath, apFlags, apFileName, apHandle);

    GetTls()->inLoadDll = false;
    return result;
}
} // namespace

#define VALIDATE(x)                                                                                                    \
    if (x != MH_OK)                                                                                                    \
        __debugbreak();

// pre eat hook?? loadmodule hook??
// the idea would be to link against an external dll, which in its init routine then hooks, so we execute before mo2?
void CoreStubsInit()
{
    s_OverridePath = TiltedPhoques::GetPath().wstring();
    std::replace(s_OverridePath.begin(), s_OverridePath.end(), L'/', L'\\');

    MH_Initialize();

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

    MH_CreateHookApi(L"ntdll.dll", "LdrGetDllHandle", &TP_LdrGetDllHandle, (void**)&RealLdrGetDllHandle);

    // TODO(Vince): we need some check if usvfs already fucked with this?
    // MH_CreateHookApi(L"ntdll.dll", "LdrGetDllFullName", &TP_LdrGetDllFullName, (void**)&RealLdrGetDllFullName);

    // TODO(Vince): same...
    MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", &TP_LdrLoadDll, (void**)&RealLdrLoadDll);
    MH_EnableHook(nullptr);

    // bool b = IsThisExeAddress((uint8_t*)0x000000018104fdf1);
}
