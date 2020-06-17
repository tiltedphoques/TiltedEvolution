#pragma once

#if TP_FALLOUT4

#include <Games/Skyrim/Havok/hkbCharacter.h>

struct BShkbHkxDB;
struct hkbBehaviorGraph;

struct BShkbAnimationGraph
{
    virtual ~BShkbAnimationGraph() {};

    uint8_t pad8[0x370 - 0x8];

    BShkbHkxDB* hkxDB; // 370
    hkbBehaviorGraph* behaviorGraph; // 378
};


#endif
