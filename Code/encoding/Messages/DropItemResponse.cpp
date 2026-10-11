#include <Messages/DropItemResponse.h>
#include <TiltedCore/Serialization.hpp>

void DropItemResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, LocalId);
    Serialization::WriteVarInt(aWriter, ServerId);
}

void DropItemResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    LocalId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
