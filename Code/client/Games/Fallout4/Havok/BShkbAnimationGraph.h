#pragma once

struct BShkbHkxDB;
struct hkbBehaviorGraph;

struct BShkbAnimationGraph
{
    virtual ~BShkbAnimationGraph() {};

    uint8_t pad8[0x370 - 0x8];

    BShkbHkxDB* hkxDB; // 370
    hkbBehaviorGraph* behaviorGraph; // 378
};
