#pragma once

#include <Games/Primitives.h>

#include <Havok/BShkbAnimationGraph.h>

struct BSFixedString;

// Fallout 4 BSAnimationGraphManager: 0xE0.
struct BSAnimationGraphManager
{
    virtual ~BSAnimationGraphManager();

    void Release()
    {
        if (InterlockedDecrement(&refCount) == 0)
            this->~BSAnimationGraphManager();
    }

    volatile LONG refCount; // 08 BSIntrusiveRefCounted
    uint8_t padC[0x40 - 0xC];
    BSTSmallArray<BShkbAnimationGraph> animationGraphs; // 40
    uint8_t pad58[0xC8 - 0x58];
    uint32_t updateLock[2];       // C8 BSSpinLock
    uint32_t dependentLock[2];    // D0 BSSpinLock
    uint32_t animationGraphIndex; // D8 uiActiveGraph
    uint32_t generateDepth;       // DC

    SortedMap<uint32_t, String> DumpAnimationVariables(bool aPrintVariables);
    uint64_t GetDescriptorKey(int aForceIndex = -1);
};

static_assert(offsetof(BSAnimationGraphManager, animationGraphs) == 0x40);
static_assert(offsetof(BSAnimationGraphManager, animationGraphIndex) == 0xD8);
static_assert(sizeof(BSAnimationGraphManager) == 0xE0);
