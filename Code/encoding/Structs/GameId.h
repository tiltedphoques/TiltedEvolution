#pragma once

#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Buffer;

struct GameId
{
    GameId() = default;
    GameId(uint32_t aModId, uint32_t aBaseId) noexcept;
    ~GameId() = default;

    bool operator==(const GameId& acRhs) const noexcept;
    bool operator!=(const GameId& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint32_t BaseId;
    uint32_t ModId;
};

namespace std
{
    template <> class hash<GameId>
    {
      public:
        size_t operator()(const GameId& gameId) const
        {
            return hash<uint32_t>()(gameId.BaseId) ^ (hash<uint32_t>()(gameId.ModId) << 1);
        }
    };
}
