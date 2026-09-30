#pragma once

struct hkbBehaviorGraph;

struct hkEventType
{
    explicit hkEventType(int32_t aType)
        : type(aType)
        , pad4(0)
        , behaviorGraph(nullptr)
        , pointer10(nullptr)
    {
    }

    int32_t type;
    uint32_t pad4;
    hkbBehaviorGraph* behaviorGraph;
    void* pointer10;
};
