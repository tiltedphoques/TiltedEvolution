
#include <intrin.h>
#include <strsafe.h>

#include <MinHook.h>
#include <FunctionHook.hpp>
#include <TiltedCore/Initializer.hpp>

#include "Launcher.h"

struct MyPEB
{
    char pad_0[2];
    bool BeingDebugged;
    char pad_3[4];
    char pad_4[8];
    PVOID ImageBaseAddress;
};

// 64 bit only
inline MyPEB* ThePeb()
{
    return reinterpret_cast<MyPEB*>(__readgsqword(0x60));
}

static fs::path s_OverridePath;

static DWORD(WINAPI* RealGetModuleFileNameW)(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
static DWORD(WINAPI* RealGetModuleFileNameA)(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
static HMODULE(WINAPI* RealGetModuleHandleW)(LPCWSTR alpModuleName);
static HMODULE(WINAPI* RealGetModuleHandleA)(LPSTR alpModuleName);

// if the module path functions are called from any TP dll within the TP bin folder we return the real path to TiltedOnline.exe
bool IsModulePathMapped(void *aOriginAddress, DWORD asize)
{
    HMODULE pModule = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(aOriginAddress), &pModule);

    if (pModule == nullptr)
        return false;

    wchar_t buf[MAX_PATH]{};
    RealGetModuleFileNameW(pModule, buf, MAX_PATH);

    return wcsstr(buf, s_OverridePath.c_str());
}

void PrintDLLOrigin(void* aOriginAddress)
{
    HMODULE pModule = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(aOriginAddress), &pModule);

    if (pModule == nullptr)
        return;

    wchar_t buf[MAX_PATH]{};
    RealGetModuleFileNameW(pModule, buf, MAX_PATH);

    std::printf("------------> %S\n", buf);
   // spdlog::debug(L"DlL origin {}\n", buf);
}

static DWORD WINAPI TP_GetModuleFileNameW(HMODULE aModule, LPWSTR alpFilename, DWORD aSize)
{
    // trampoline space for USVFS
    TP_EMPTY_HOOK_PLACEHOLDER;

    if (aModule == nullptr || aModule == ThePeb()->ImageBaseAddress)
    {
        void* rbp = _ReturnAddress(); 

        if (!IsModulePathMapped(rbp, aSize))
        {
            auto& aExePath = GetLauncher()->GetExePath();
            StringCchCopyW(alpFilename, aSize, aExePath.c_str());

            return static_cast<DWORD>(std::wcslen(alpFilename));
        }

        PrintDLLOrigin(rbp);

    }


    return RealGetModuleFileNameW(aModule, alpFilename, aSize);
}

static DWORD WINAPI TP_GetModuleFileNameA(HMODULE aModule, LPSTR alpFileName, DWORD aSize)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    if (aModule == nullptr || aModule == ThePeb()->ImageBaseAddress)
    {
        void* rbp = _ReturnAddress();

        if (!IsModulePathMapped(rbp, aSize))
        {

            auto aExePath = GetLauncher()->GetExePath().u8string();
            StringCchCopyA(alpFileName, aSize, aExePath.c_str());

            return static_cast<DWORD>(std::strlen(alpFileName));
        }

        PrintDLLOrigin(rbp);
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

static TiltedPhoques::Initializer s_Init([] {
   s_OverridePath = TiltedPhoques::GetPath();

   MH_Initialize();
   MH_CreateHookApi(L"Kernel32.dll", "GetModuleFileNameW", &TP_GetModuleFileNameW, (void**)&RealGetModuleFileNameW);
   MH_CreateHookApi(L"Kernel32.dll", "GetModuleFileNameA", &TP_GetModuleFileNameA, (void**)&RealGetModuleFileNameA);

   MH_CreateHookApi(L"Kernel32.dll", "GetModuleHandleW", &TP_GetModuleHandleW, (void**)&RealGetModuleHandleW);
   MH_CreateHookApi(L"Kernel32.dll", "GetModuleHandleA", &TP_GetModuleHandleA, (void**)&RealGetModuleHandleA);
   //MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", LdrLoadDllStub, (void**)&g_origLoadDll);

   MH_EnableHook(nullptr);
});
