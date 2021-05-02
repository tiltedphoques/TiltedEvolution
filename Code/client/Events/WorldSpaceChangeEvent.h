#pragma once

struct WorldSpaceChangeEvent
{
    explicit WorldSpaceChangeEvent(const uint32_t aWorldSpaceId)
        : WorldSpaceId(aWorldSpaceId)
    {}

    uint32_t WorldSpaceId;
    Vector<uint32_t> Cells{};
};
