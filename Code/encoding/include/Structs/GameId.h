#pragma once

#include <Buffer.hpp>

using TiltedPhoques::Buffer;

struct GameId
{
    GameId() = default;
    ~GameId() = default;

    bool operator==(const GameId& acRhs) const noexcept;
    bool operator!=(const GameId& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint32_t BaseId;
    uint32_t ModId;
};
