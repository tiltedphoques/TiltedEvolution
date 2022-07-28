#pragma once

struct MoveActorEvent
{
    MoveActorEvent(uint32_t aFormId, uint32_t aCellId, NiPoint3 aPosition)
        : FormId(aFormId), CellId(aCellId), Position(aPosition)
    {
    }

    uint32_t FormId;
    uint32_t CellId;
    NiPoint3 Position;
};
