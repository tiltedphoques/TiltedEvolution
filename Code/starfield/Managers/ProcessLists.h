#pragma once

#include <BSCore/BSTArray.h>

struct ProcessLists
{
    static ProcessLists* Get()
    {
        return (ProcessLists*)0x1456ED250; // 878338
    }

    virtual void unk0();
    // ...

    uint8_t unk8[0x58 - 0x8];
    BSTArray<uint32_t> highActorHandleArray;
    // ...
};
