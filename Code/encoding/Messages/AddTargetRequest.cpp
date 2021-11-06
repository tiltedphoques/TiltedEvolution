#include <Messages/AddTargetRequest.h>

void AddTargetRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, TargetId);
    Serialization::WriteVarInt(aWriter, SpellId);
}

void AddTargetRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    TargetId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SpellId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
