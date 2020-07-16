#include <Structs/GameId.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool GameId::operator==(const GameId& acRhs) const noexcept
{
    return BaseId == acRhs.BaseId &&
        ModId == acRhs.ModId;
}

bool GameId::operator!=(const GameId& acRhs) const noexcept
{
    return !this->operator==(acRhs);
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
