#pragma once

struct WorldSpaceChangeEvent
{
    explicit WorldSpaceChangeEvent(const uint32_t aWorldSpaceId)
        : WorldSpaceId(aWorldSpaceId)
    {}

    uint32_t WorldSpaceId;
    GameId PlayerCell;
    int32_t CurrentGridX;
    int32_t CurrentGridY;
    Vector<GameId> Cells{};
};
