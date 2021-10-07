// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

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
    
    private:
      void* m_pBlock = nullptr;
    };
}
