#pragma once

#include <Windows.h>

constexpr uintptr_t kLowRipZoneSize = 0x10000; // 1024 * 64
constexpr uintptr_t kHighRipSize = 0x100000;   // 1024 * 1024
constexpr uintptr_t kLowRipStart = 0x80000000 + (kHighRipSize * 128);

// reserve memory for certain native code based mods
bool LowRipZoneInit()
{
    uintptr_t mainAddress = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr)) - 1;

    MEMORY_BASIC_INFORMATION info{};

    // we go bottom to top
    uintptr_t ripAddress = kLowRipStart;
    while ((ripAddress + kLowRipZoneSize) < mainAddress)
    {
        void* pRipSegment = reinterpret_cast<void*>(ripAddress);

        if (!VirtualQuery(pRipSegment, &info, sizeof(info)))
        {
            return false;
        }

        if (info.State == MEM_FREE)
        {
            if (VirtualAlloc(pRipSegment, kLowRipZoneSize, MEM_RESERVE, PAGE_EXECUTE_READWRITE))
            {
                return true;
            }
        }

        ripAddress += kLowRipZoneSize;
    }

    return false;
}
