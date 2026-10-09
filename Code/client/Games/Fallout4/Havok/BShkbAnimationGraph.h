#pragma once

struct BShkbHkxDB;
struct hkbBehaviorGraph;

// Fallout 4 BShkbAnimationGraph: 0x3D0.
struct BShkbAnimationGraph
{
    virtual ~BShkbAnimationGraph(){};

    uint8_t pad8[0x370 - 0x8];
    BShkbHkxDB* hkxDB;               // 370 BShkbHkxDB::ProjectDBData
    hkbBehaviorGraph* behaviorGraph; // 378
    uint8_t pad380[0x3D0 - 0x380];
};

static_assert(offsetof(BShkbAnimationGraph, hkxDB) == 0x370);
static_assert(offsetof(BShkbAnimationGraph, behaviorGraph) == 0x378);
static_assert(sizeof(BShkbAnimationGraph) == 0x3D0);
