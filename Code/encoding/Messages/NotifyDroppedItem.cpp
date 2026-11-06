#include <Messages/NotifyDroppedItem.h>

void NotifyDroppedItem::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, DropperId);
    Serialization::WriteVarInt(aWriter, DropId);
    Item.Serialize(aWriter);
    Position.Serialize(aWriter);
    CellId.Serialize(aWriter);
    WorldSpaceId.Serialize(aWriter);
}

void NotifyDroppedItem::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    DropperId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    DropId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Item.Deserialize(aReader);
    Position.Deserialize(aReader);
    CellId.Deserialize(aReader);
    WorldSpaceId.Deserialize(aReader);
}
