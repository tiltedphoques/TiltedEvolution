#include <Messages/RequestDroppedItemPickup.h>

void RequestDroppedItemPickup::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, DropperId);
    Serialization::WriteVarInt(aWriter, DropId);
}

void RequestDroppedItemPickup::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    DropperId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    DropId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
