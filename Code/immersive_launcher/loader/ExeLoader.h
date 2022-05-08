#pragma once

/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

// Changes:
// - 2021/2/25: Added CEG decryption method.

#include <Windows.h>
#include <cstdint>

class ExeLoader
{
  public:
    using TEntryPoint = void(*)();
    using TFuncHandler = FARPROC(*)(HMODULE, const char*);

    explicit ExeLoader(uint32_t aLoadLimit, TFuncHandler aFuncPtr = GetProcAddress);

    bool Load(const uint8_t *apProgramBuffer);

    TEntryPoint GetEntryPoint() const
    {
        return static_cast<TEntryPoint>(m_pEntryPoint);
    }

  private:
    void LoadSections(const IMAGE_NT_HEADERS* apNtHeader);
    void LoadImports(const IMAGE_NT_HEADERS* apNtHeader);
    void LoadTLS(const IMAGE_NT_HEADERS* apNtHeader, const IMAGE_NT_HEADERS* apSourceNt);
    void LoadExceptionTable(IMAGE_NT_HEADERS* apNtHeader);
    void DecryptCeg(IMAGE_NT_HEADERS* apSourceNt);

    template <typename T> inline T* GetRVA(uint32_t aRva)
    {
        return (T*)(m_pBinary + aRva);
    }

    template <typename T> inline T* GetOffset(uint32_t aRva)
    {
        return (T*)(m_pBinary + Rva2Offset(aRva));
    }

    template <typename T> inline T* GetTargetRVA(uint32_t aRva)
    {
        return (T*)((uint8_t*)m_moduleHandle + aRva);
    }

  private:
    uint32_t Rva2Offset(uint32_t aRva) noexcept;

    const uint8_t* m_pBinary = nullptr;
    const TFuncHandler m_pFuncHandler = nullptr;

    uint32_t m_loadLimit;
    HMODULE m_moduleHandle = nullptr;
    void* m_pEntryPoint = nullptr;
};
