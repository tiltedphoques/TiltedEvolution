#pragma once

#include <Games/Primitives.h>
#include <BSCore/BSAtomic.h>
#include <BSCore/BSIntrusiveRefCounted.h>

#include <Havok/BShkbAnimationGraph.h>

struct BSFixedString;

struct BSAnimationGraphManager : BSIntrusiveRefCounted
{
    virtual ~BSAnimationGraphManager();
    virtual void sub_1(void* apUnk1);

    void* pad_ptrs[6];
    BSTSmallArray<BShkbAnimationGraph> animationGraphs; // 40 - 20
    void* pad_ptrs2[9];
    BSSpinLock lock;       // 98 - 4C
    void* unkPtrAfterLock; // A0 - 58

#if TP_PLATFORM_32
    void* unkPtrOldrim;
#endif

    uint32_t animationGraphIndex; // A8 - 5C

    SortedMap<uint32_t, String> DumpAnimationVariables(bool aPrintVariables);
    uint64_t GetDescriptorKey(int aForceIndex = -1);
    uint64_t GetProjectKey(int aForceIndex = -1);
};

#if TP_PLATFORM_64
static_assert(offsetof(BSAnimationGraphManager, refCount) == 0x8);
static_assert(offsetof(BSAnimationGraphManager, animationGraphs) == 0x40);
static_assert(offsetof(BSAnimationGraphManager, lock) == 0xA0);
static_assert(offsetof(BSAnimationGraphManager, animationGraphIndex) == 0xB0);
#else
static_assert(offsetof(BSAnimationGraphManager, refCount) == 0x4);
static_assert(offsetof(BSAnimationGraphManager, animationGraphs) == 0x20);
static_assert(offsetof(BSAnimationGraphManager, lock) == 0x4C);
static_assert(offsetof(BSAnimationGraphManager, animationGraphIndex) == 0x5C);
#endif
