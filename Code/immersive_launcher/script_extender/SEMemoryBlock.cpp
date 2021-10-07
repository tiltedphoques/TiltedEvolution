// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <Windows.h>
#include <cstdint>
#include "SEMemoryBlock.h"

namespace script_extender
{
static constexpr uint32_t kBlockLength = 1024 * 64;

SEMemoryBlock::SEMemoryBlock()
{
    uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    uintptr_t addr = moduleBase;
    uintptr_t maxDisplacement =
        0x80000000 - (1024 * 1024 * 128); // largest 32-bit displacement with 128MB scratch space
    uintptr_t lowestOKAddress = (moduleBase >= maxDisplacement) ? moduleBase - maxDisplacement : 0;
    addr--;

    while (!m_pBlock)
    {
        MEMORY_BASIC_INFORMATION info;
        if (!VirtualQuery((void*)addr, &info, sizeof(info)))
        {
            break;
        }

        if (info.State == MEM_FREE)
        {
            if (info.RegionSize >= kBlockLength)
            {
                // try to allocate it
                addr = ((uintptr_t)info.BaseAddress) + info.RegionSize - kBlockLength;
                m_pBlock = (void*)VirtualAlloc((void*)addr, kBlockLength, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            }
        }

        // move back and try again
        if (!m_pBlock)
        {
            addr = ((uintptr_t)info.BaseAddress) - 1;
        }
    }
}

SEMemoryBlock::~SEMemoryBlock()
{
    if (m_pBlock)
        VirtualFree(m_pBlock, kBlockLength, MEM_RELEASE);
}
}
