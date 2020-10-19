#pragma once

#include <Havok/hkbCharacter.h>

struct BSFixedString;
struct BShkbHkxDB;
struct hkbBehaviorGraph;
struct bhkWorldM;

struct BShkbAnimationGraph
{
    virtual ~BShkbAnimationGraph() {};

    uint8_t pad8[0xC0 - 0x8];
    hkbCharacter character;
    uint8_t pad160[0x200 - (0xC0 + sizeof(hkbCharacter))];
    BShkbHkxDB* hkxDB;
    hkbBehaviorGraph* behaviorGraph; // 208
    uint8_t pad210[0x238 - 0x210];
    bhkWorldM* hkWorldM; // 238

    // Re do not use
    bool ReSendEvent(BSFixedString* apEventName);
};
