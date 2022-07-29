#include <Structs/GameId.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

GameId::GameId(uint32_t aModId, uint32_t aBaseId) noexcept : ModId(aModId), BaseId(aBaseId)
{
}

bool GameId::operator==(const GameId& acRhs) const noexcept
{
    return BaseId == acRhs.BaseId && ModId == acRhs.ModId;
}

bool GameId::operator!=(const GameId& acRhs) const noexcept
{
    return !this->operator==(acRhs);
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

uint32_t GameId::ToUint32() const noexcept
{
    return ModId * 0x01000000 + BaseId;
}

GameId GameId::FromUint32(uint32_t aValue) noexcept
{
    GameId result;
    result.ModId = aValue / 0x01000000;
    result.BaseId = aValue % 0x01000000;
    return result;
}
