#include <Messages/DropItemRequest.h>

void DropItemRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, RefId);
    Data.Serialize(aWriter);
}

void DropItemRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    RefId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Data.Deserialize(aReader);
}
