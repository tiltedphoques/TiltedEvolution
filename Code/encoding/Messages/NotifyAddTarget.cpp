#include <Messages/NotifyAddTarget.h>

void NotifyAddTarget::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, TargetId);
    Serialization::WriteVarInt(aWriter, SpellId);
}

void NotifyAddTarget::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    TargetId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SpellId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
