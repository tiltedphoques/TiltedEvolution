/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

// Changes:
// - 2021/2/24: Moved TLS routine.
// - 2021/2/25: Implemented CEG decryption method.

#define SPDLOG_WCHAR_FILENAMES
#include <TiltedCore/Filesystem.hpp>
#include <spdlog/formatter.h>
#include <winternl.h>

#include "ExeLoader.h"
#include "steam/SteamCeg.h"
#include "utils/Error.h"
#include "utils/NtInternal.h"

#if defined(_M_AMD64)
typedef enum _FUNCTION_TABLE_TYPE
{
    RF_SORTED,
    RF_UNSORTED,
    RF_CALLBACK
} FUNCTION_TABLE_TYPE;

typedef struct _DYNAMIC_FUNCTION_TABLE
{
    LIST_ENTRY Links;
    PRUNTIME_FUNCTION FunctionTable;
    LARGE_INTEGER TimeStamp;

    ULONG_PTR MinimumAddress;
    ULONG_PTR MaximumAddress;
    ULONG_PTR BaseAddress;

    PGET_RUNTIME_FUNCTION_CALLBACK Callback;
    PVOID Context;
    PWSTR OutOfProcessCallbackDll;
    FUNCTION_TABLE_TYPE Type;
    ULONG EntryCount;
} DYNAMIC_FUNCTION_TABLE, *PDYNAMIC_FUNCTION_TABLE;
#endif

// TODO: move me to wstring util..
std::wstring ConvertStringToWstring(const std::string_view str)
{
    int nChars = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, str.data(), static_cast<int>(str.length()), NULL, 0);

    std::wstring wstrTo;
    if (nChars)
    {
        wstrTo.resize(nChars);
        if (MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, str.data(), static_cast<int>(str.length()), &wstrTo[0],
                                nChars))
        {
            return wstrTo;
        }
    }
    return {};
}

ExeLoader::ExeLoader(uint32_t aLoadLimit, TFuncHandler aFuncHandler)
    : m_loadLimit(aLoadLimit), m_pFuncHandler(aFuncHandler)
{
}

void ExeLoader::LoadImports(const IMAGE_NT_HEADERS* apNtHeader)
{
    const auto* importDirectory = &apNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    const auto* descriptor = GetTargetRVA<const IMAGE_IMPORT_DESCRIPTOR>(importDirectory->VirtualAddress);

    while (descriptor->Name)
    {
        auto dllName = ConvertStringToWstring(GetTargetRVA<char>(descriptor->Name));

        HMODULE hMod = LoadLibraryW(dllName.c_str());
        if (!hMod)
        {
            auto msg = fmt::format(L"Failed to find dll: {}", dllName);
            Die(msg.c_str(), true);
            continue;
        }

        // "don't load"
        if (*reinterpret_cast<uint32_t*>(hMod) == 0xFFFFFFFF)
        {
            descriptor++;
            continue;
        }

        auto nameTableEntry = GetTargetRVA<uintptr_t>(descriptor->OriginalFirstThunk);
        auto addressTableEntry = GetTargetRVA<uintptr_t>(descriptor->FirstThunk);

        if (!descriptor->OriginalFirstThunk)
        {
            nameTableEntry = GetTargetRVA<uintptr_t>(descriptor->FirstThunk);
        }

        while (*nameTableEntry)
        {
            FARPROC function;
            const char* functionName{nullptr};

            // is this an ordinal-only import?
            if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
            {
                function = GetProcAddress(hMod, MAKEINTRESOURCEA(IMAGE_ORDINAL(*nameTableEntry)));
            }
            else
            {
                auto import = GetTargetRVA<IMAGE_IMPORT_BY_NAME>(static_cast<uint32_t>(*nameTableEntry));

                function = m_pFuncHandler(hMod, import->Name);
                functionName = import->Name;
            }

            if (!function)
            {
                wchar_t pathName[1024]{};
                GetModuleFileNameW(hMod, pathName, sizeof(pathName) - 1);

                auto thunkName = ConvertStringToWstring(functionName ? functionName : "<unknown>");
                auto msg = fmt::format(L"Failed to find thunk {} in dll {}", thunkName, pathName);
                Die(msg.c_str(), true);
            }

            *addressTableEntry = (uintptr_t)function;

            nameTableEntry++;
            addressTableEntry++;
        }

        descriptor++;
    }
}

void ExeLoader::LoadSections(const IMAGE_NT_HEADERS* apNtHeader)
{
    auto* section = IMAGE_FIRST_SECTION(apNtHeader);
    for (int i = 0; i < apNtHeader->FileHeader.NumberOfSections; i++)
    {
        uint8_t* targetAddress = GetTargetRVA<uint8_t>(section->VirtualAddress);
        const void* sourceAddress = m_pBinary + section->PointerToRawData;

        if (targetAddress >= (reinterpret_cast<uint8_t*>(m_moduleHandle) + m_loadLimit))
        {
            return;
        }

        if (section->SizeOfRawData > 0)
        {
            uint32_t sizeOfData = std::min(section->SizeOfRawData, section->Misc.VirtualSize);

            memcpy(targetAddress, sourceAddress, sizeOfData);

            DWORD oldProtect;
            VirtualProtect(targetAddress, sizeOfData, PAGE_EXECUTE_READWRITE, &oldProtect);
        }

        section++;
    }
}

void ExeLoader::LoadTLS(const IMAGE_NT_HEADERS* apNtHeader, const IMAGE_NT_HEADERS* apSourceNt)
{
    if (apNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
    {
        const auto* sourceTls = GetTargetRVA<IMAGE_TLS_DIRECTORY>(
            apNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
        const auto* targetTls = GetTargetRVA<IMAGE_TLS_DIRECTORY>(
            apSourceNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

        *(DWORD*)(sourceTls->AddressOfIndex) = 0;

        LPVOID tlsBase = *(LPVOID*)__readgsqword(0x58);

        DWORD oldProtect;
        VirtualProtect(reinterpret_cast<LPVOID>(targetTls->StartAddressOfRawData),
                       sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData, PAGE_READWRITE, &oldProtect);

        std::memcpy(tlsBase, reinterpret_cast<void*>(sourceTls->StartAddressOfRawData),
                    sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData);
        std::memcpy((void*)targetTls->StartAddressOfRawData, reinterpret_cast<void*>(sourceTls->StartAddressOfRawData),
                    sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData);
    }
}

void ExeLoader::LoadExceptionTable(IMAGE_NT_HEADERS* apNtHeader)
{
    IMAGE_DATA_DIRECTORY* exceptionDirectory =
        &apNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];

    RUNTIME_FUNCTION* functionList = GetTargetRVA<RUNTIME_FUNCTION>(exceptionDirectory->VirtualAddress);
    DWORD entryCount = exceptionDirectory->Size / sizeof(RUNTIME_FUNCTION);

    // has no use - inverted function tables get used instead from Ldr; we have no influence on those
    if (!RtlAddFunctionTable(functionList, entryCount, (DWORD64)GetModuleHandle(nullptr)))
    {
        Die(L"Setting exception handlers failed.", false);
    }

    // replace the function table stored for debugger purposes (though we just added it above)
    {
        PLIST_ENTRY(NTAPI * rtlGetFunctionTableListHead)(VOID);
        rtlGetFunctionTableListHead = (decltype(rtlGetFunctionTableListHead))GetProcAddress(
            GetModuleHandleW(L"ntdll.dll"), "RtlGetFunctionTableListHead");

        if (rtlGetFunctionTableListHead)
        {
            auto tableListHead = rtlGetFunctionTableListHead();
            auto tableListEntry = tableListHead->Flink;

            while (tableListEntry != tableListHead)
            {
                auto functionTable = CONTAINING_RECORD(tableListEntry, DYNAMIC_FUNCTION_TABLE, Links);

                if (functionTable->BaseAddress == reinterpret_cast<ULONG_PTR>(m_moduleHandle))
                {
                    if (functionTable->FunctionTable != functionList)
                    {
                        DWORD oldProtect;
                        VirtualProtect(functionTable, sizeof(DYNAMIC_FUNCTION_TABLE), PAGE_READWRITE, &oldProtect);

                        functionTable->EntryCount = entryCount;
                        functionTable->FunctionTable = functionList;

                        VirtualProtect(functionTable, sizeof(DYNAMIC_FUNCTION_TABLE), oldProtect, &oldProtect);
                    }
                }

                tableListEntry = functionTable->Links.Flink;
            }
        }
    }
}

uint32_t ExeLoader::Rva2Offset(uint32_t aRva) noexcept
{
    const auto* dos = GetRVA<const IMAGE_DOS_HEADER>(0);
    const auto* nt = GetRVA<const IMAGE_NT_HEADERS>(dos->e_lfanew);

    auto* section = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++)
    {
        if (aRva >= section[i].VirtualAddress && (aRva < section[i].VirtualAddress + section[i].Misc.VirtualSize))
        {
            return static_cast<uint32_t>(aRva - (section[i].VirtualAddress - section[i].PointerToRawData));
        }
    }

    return 0;
}

void ExeLoader::DecryptCeg(IMAGE_NT_HEADERS* apSourceNt)
{
    auto entry = apSourceNt->OptionalHeader.AddressOfEntryPoint;
    // analyze executable sections if the entry point is already protected
    if (*GetOffset<uint32_t>(entry) != 0x000000e8)
        return;

    const auto* section = IMAGE_FIRST_SECTION(apSourceNt);
    for (int i = 0; i < apSourceNt->FileHeader.NumberOfSections; i++)
    {
        if (!_strcmpi(reinterpret_cast<const char*>(section[i].Name), ".text"))
        {
            break;
        }
    }

    steam::CEGLocationInfo info{GetOffset<uint8_t>(entry),
                                {GetOffset<uint8_t>(section->VirtualAddress), section->SizeOfRawData}};

    auto realEntry = steam::CrackCEGInPlace(info);

    apSourceNt->FileHeader.NumberOfSections--;
    apSourceNt->OptionalHeader.AddressOfEntryPoint = static_cast<uint32_t>(realEntry);
}

bool ExeLoader::Load(const uint8_t* apProgramBuffer)
{
    m_pBinary = apProgramBuffer;
    m_moduleHandle = GetModuleHandleW(nullptr);

    // validate the target
    const auto* dosHeader = GetRVA<const IMAGE_DOS_HEADER>(0);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return false;
    }

    // remove protections
    auto* ntHeader = GetRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);
    DecryptCeg(ntHeader);

    // these point to launcher.exe's headers
    auto* sourceHeader = GetTargetRVA<IMAGE_DOS_HEADER>(0);
    auto* sourceNtHeader = GetTargetRVA<IMAGE_NT_HEADERS>(sourceHeader->e_lfanew);

    // store EP
    m_pEntryPoint = GetTargetRVA<void>(ntHeader->OptionalHeader.AddressOfEntryPoint);

    // store these as they will get overridden by the target's header
    // but we really need them in order to not break debugging for cosi.
    auto sourceChecksum = sourceNtHeader->OptionalHeader.CheckSum;
    auto sourceTimestamp = sourceNtHeader->FileHeader.TimeDateStamp;
    auto sourceDebugDir = sourceNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];

    LoadSections(ntHeader);
    LoadImports(ntHeader);
#if defined(_M_AMD64)
    LoadExceptionTable(ntHeader);
    LoadTLS(ntHeader, sourceNtHeader);
#endif

    // copy over the offset to the new imports directory
    DWORD oldProtect;
    VirtualProtect(sourceNtHeader, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);

    // re-target the import directory to the target's; ours isn't needed anymore.
    sourceNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] =
        ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    const size_t ntCompleteHeaderSize =
        sizeof(IMAGE_NT_HEADERS) + (ntHeader->FileHeader.NumberOfSections * (sizeof(IMAGE_SECTION_HEADER)));

    // overwrite our headers with the target headers
    std::memcpy(sourceNtHeader, ntHeader, ntCompleteHeaderSize);

    // good old switcheroo
    // TODO: consider making this optional to allow loading the game's pdb.
    sourceNtHeader->OptionalHeader.CheckSum = sourceChecksum;
    sourceNtHeader->FileHeader.TimeDateStamp = sourceTimestamp;
    sourceNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG] = sourceDebugDir;

    m_pBinary = nullptr;
    return true;
}
