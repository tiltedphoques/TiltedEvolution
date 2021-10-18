// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "SEMemoryBlock.h"
#include <Windows.h>
#include <cstdint>

namespace script_extender
{
constexpr uintptr_t kLowRipZoneSize = 0x10000; // 1024 * 64
constexpr uintptr_t kHighRipSize = 0x100000;   // 1024 * 1024
constexpr uintptr_t kLowRipStart = 0x80000000 + (kHighRipSize * 128);

SEMemoryBlock::SEMemoryBlock()
{
    uintptr_t mainAddress = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr)) - 1;

    MEMORY_BASIC_INFORMATION info{};

    // we go bottom to top
    uintptr_t ripAddress = kLowRipStart;
    while ((ripAddress + kLowRipZoneSize) < mainAddress)
    {
        void* pRipSegment = reinterpret_cast<void*>(ripAddress);
        if (!VirtualQuery(pRipSegment, &info, sizeof(info)))
            return;

        if (info.State == MEM_FREE)
        {
            if (m_pBlock = VirtualAlloc(pRipSegment, kLowRipZoneSize, MEM_RESERVE, PAGE_EXECUTE_READWRITE))
                return;
        }

        ripAddress += kLowRipZoneSize;
    }

    m_lastCanidate = ripAddress;
}

SEMemoryBlock::~SEMemoryBlock()
{
    if (m_pBlock)
        VirtualFree(m_pBlock, kLowRipZoneSize, MEM_RELEASE);
}
} // namespace script_extender
