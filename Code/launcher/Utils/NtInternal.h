#pragma once

#include <intrin.h>

namespace NtInternal
{
    struct MyPEB
    {
        char pad_0[2];
        bool bBeingDebugged;
        char pad_3[4];
        char pad_4[8];
        void* pImageBase;
    };
    
    // 64 bit only
    inline MyPEB* ThePeb()
    {
        return reinterpret_cast<MyPEB*>(__readgsqword(0x60));
    }
}
