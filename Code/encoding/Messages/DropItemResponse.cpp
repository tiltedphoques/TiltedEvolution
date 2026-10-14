#include <Messages/DropItemResponse.h>

void DropItemResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, RefId);
    Serialization::WriteVarInt(aWriter, ServerId);
}

void DropItemResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    RefId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
