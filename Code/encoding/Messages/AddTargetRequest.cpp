#include <Messages/AddTargetRequest.h>

void AddTargetRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, TargetId);
    SpellId.Serialize(aWriter);
    EffectId.Serialize(aWriter);
    Serialization::WriteFloat(aWriter, Magnitude);
}

void AddTargetRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    TargetId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SpellId.Deserialize(aReader);
    EffectId.Deserialize(aReader);
    Magnitude = Serialization::ReadFloat(aReader);
}
