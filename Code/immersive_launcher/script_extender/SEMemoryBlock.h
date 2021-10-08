// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>

namespace script_extender
{
    class SEMemoryBlock final 
    {
    public:
        SEMemoryBlock();
        ~SEMemoryBlock();

        inline bool Good()
        {
            return m_pBlock;
        }

        inline uintptr_t LastRip() const
        {
            return m_lastCanidate;
        }
    
    private:
      void* m_pBlock = nullptr;
      uintptr_t m_lastCanidate = 0;
    };
}
