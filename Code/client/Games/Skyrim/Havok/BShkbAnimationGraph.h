#pragma once

#include <Havok/hkbCharacter.h>
#include <Misc/BSFixedString.h>

struct BShkbHkxDB;
struct hkbBehaviorGraph;
struct bhkWorldM;

struct BShkbAnimationGraph
{
    virtual ~BShkbAnimationGraph(){};

    uint8_t pad8[0xC0 - 0x8];
    hkbCharacter character;
    uint8_t pad160[0x1F0 - (0xC0 + sizeof(hkbCharacter))];
    BSFixedString projectName; // 1F0
    uint8_t pad1F8[0x200 - 0x1F8];
    BShkbHkxDB* hkxDB;               // 200
    hkbBehaviorGraph* behaviorGraph; // 208
    uint8_t pad210[0x238 - 0x210];
    bhkWorldM* hkWorldM; // 238
};

static_assert(offsetof(BShkbAnimationGraph, projectName) == 0x1F0);
static_assert(offsetof(BShkbAnimationGraph, hkxDB) == 0x200);
static_assert(offsetof(BShkbAnimationGraph, behaviorGraph) == 0x208);
