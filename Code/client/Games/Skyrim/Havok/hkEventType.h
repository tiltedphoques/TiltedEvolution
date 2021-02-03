#pragma once

struct hkbBehaviorGraph;

struct hkEventType
{
    hkEventType(int32_t aType);
    hkEventType(const hkEventType&) = default;

    int32_t type;
    uint32_t pad4;
    hkbBehaviorGraph* behaviorGraph;
    void* pointer10;
};
