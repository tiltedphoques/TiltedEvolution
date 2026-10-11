#include <Structs/DroppedItemData.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool DroppedItemTransform::operator==(const DroppedItemTransform& acRhs) const noexcept
{
    return Position == acRhs.Position && Rotation == acRhs.Rotation;
}

bool DroppedItemTransform::operator!=(const DroppedItemTransform& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void DroppedItemTransform::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Position.Serialize(aWriter);
    Serialization::WriteFloat(aWriter, Rotation.x);
    Serialization::WriteFloat(aWriter, Rotation.y);
    Serialization::WriteFloat(aWriter, Rotation.z);
}

void DroppedItemTransform::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Position.Deserialize(aReader);
    Rotation.x = Serialization::ReadFloat(aReader);
    Rotation.y = Serialization::ReadFloat(aReader);
    Rotation.z = Serialization::ReadFloat(aReader);
}

bool DroppedItemMove::operator==(const DroppedItemMove& acRhs) const noexcept
{
    return ServerId == acRhs.ServerId && Transform == acRhs.Transform && IsAtRest == acRhs.IsAtRest;
}

bool DroppedItemMove::operator!=(const DroppedItemMove& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void DroppedItemMove::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Transform.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsAtRest);
}

void DroppedItemMove::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Transform.Deserialize(aReader);
    IsAtRest = Serialization::ReadBool(aReader);
}

bool DroppedItemData::operator==(const DroppedItemData& acRhs) const noexcept
{
    return ServerId == acRhs.ServerId && Item == acRhs.Item && CellId == acRhs.CellId && WorldSpaceId == acRhs.WorldSpaceId && Transform == acRhs.Transform && IsAtRest == acRhs.IsAtRest;
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
    Transform.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsAtRest);
}

void DroppedItemData::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Item.Deserialize(aReader);
    CellId.Deserialize(aReader);
    WorldSpaceId.Deserialize(aReader);
    Transform.Deserialize(aReader);
    IsAtRest = Serialization::ReadBool(aReader);
}
