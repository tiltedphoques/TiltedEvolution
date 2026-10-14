#include <Structs/DroppedItemData.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool DroppedItemData::operator==(const DroppedItemData& acRhs) const noexcept
{
    return ServerId == acRhs.ServerId && Item == acRhs.Item && CellId == acRhs.CellId && WorldSpaceId == acRhs.WorldSpaceId && PositionX == acRhs.PositionX && PositionY == acRhs.PositionY && PositionZ == acRhs.PositionZ &&
           RotationX == acRhs.RotationX && RotationY == acRhs.RotationY && RotationZ == acRhs.RotationZ;
}

bool DroppedItemData::operator!=(const DroppedItemData& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void DroppedItemData::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Item.Serialize(aWriter);
    CellId.Serialize(aWriter);
    WorldSpaceId.Serialize(aWriter);
    Serialization::WriteFloat(aWriter, PositionX);
    Serialization::WriteFloat(aWriter, PositionY);
    Serialization::WriteFloat(aWriter, PositionZ);
    Serialization::WriteFloat(aWriter, RotationX);
    Serialization::WriteFloat(aWriter, RotationY);
    Serialization::WriteFloat(aWriter, RotationZ);
}

void DroppedItemData::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Item.Deserialize(aReader);
    CellId.Deserialize(aReader);
    WorldSpaceId.Deserialize(aReader);
    PositionX = Serialization::ReadFloat(aReader);
    PositionY = Serialization::ReadFloat(aReader);
    PositionZ = Serialization::ReadFloat(aReader);
    RotationX = Serialization::ReadFloat(aReader);
    RotationY = Serialization::ReadFloat(aReader);
    RotationZ = Serialization::ReadFloat(aReader);
}
