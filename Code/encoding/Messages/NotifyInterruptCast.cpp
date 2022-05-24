#include <Messages/NotifyInterruptCast.h>

void NotifyInterruptCast::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, CasterId);
    Serialization::WriteVarInt(aWriter, CastingSource);
}

void NotifyInterruptCast::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    CasterId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CastingSource = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
