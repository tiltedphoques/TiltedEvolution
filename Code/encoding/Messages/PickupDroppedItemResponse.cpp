#include <Messages/PickupDroppedItemResponse.h>

void PickupDroppedItemResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteBool(aWriter, Accepted);
}

void PickupDroppedItemResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Accepted = Serialization::ReadBool(aReader);
}
