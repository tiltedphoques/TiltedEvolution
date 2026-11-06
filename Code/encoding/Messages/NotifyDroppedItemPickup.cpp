#include <Messages/NotifyDroppedItemPickup.h>

void NotifyDroppedItemPickup::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, DropperId);
    Serialization::WriteVarInt(aWriter, DropId);
    aWriter.WriteBits(Result, 8);
}

void NotifyDroppedItemPickup::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    DropperId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    DropId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    uint64_t result = 0;
    aReader.ReadBits(result, 8);
    Result = static_cast<uint8_t>(result);
}
