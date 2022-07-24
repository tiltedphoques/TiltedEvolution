#include <Structs/GameId.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

GameId::GameId(uint32_t aModId, uint32_t aBaseId) noexcept
    : ModId(aModId)
    , BaseId(aBaseId)
{
}

GameId::GameId(uint64_t aSerializedId) noexcept
{
    BaseId = aSerializedId & 0xFFFFFFFF;
    ModId = aSerializedId >> 32;
}

bool GameId::operator==(const GameId& acRhs) const noexcept
{
    return BaseId == acRhs.BaseId &&
        ModId == acRhs.ModId;
}

bool GameId::operator!=(const GameId& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

uint64_t GameId::AsUint64() const noexcept
{
    uint64_t res = ModId;
    res = (res << 32) | BaseId;
    return res;
}

GameId::operator bool() const noexcept
{
    return *this != GameId{};
}

void GameId::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, BaseId);
    Serialization::WriteVarInt(aWriter, ModId);
}

void GameId::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    BaseId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ModId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
