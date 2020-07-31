#pragma once

#include <Structs/GameId.h>

struct Faction
{
    Faction() = default;
    ~Faction() = default;

    bool operator==(const Faction& acRhs) const noexcept;
    bool operator!=(const Faction& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    GameId Id;
    int8_t Rank;
};
