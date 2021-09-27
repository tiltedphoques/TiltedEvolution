#include <Messages/NotifySpellCast.h>

void NotifySpellCast::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, CasterId);
    Serialization::WriteVarInt(aWriter, CastingSource);
    Serialization::WriteBool(aWriter, IsDualCasting);
    Serialization::WriteVarInt(aWriter, DesiredTarget);
}

void NotifySpellCast::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    CasterId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CastingSource = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    IsDualCasting = Serialization::ReadBool(aReader);
    DesiredTarget = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
