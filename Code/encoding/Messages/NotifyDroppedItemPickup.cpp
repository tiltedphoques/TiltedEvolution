#include <Messages/NotifyDroppedItemPickup.h>

void NotifyDroppedItemPickup::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, DropperId);
    Serialization::WriteVarInt(aWriter, DropId);
}

void NotifyDroppedItemPickup::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    DropperId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    DropId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
