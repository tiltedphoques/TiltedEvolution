#include <Messages/NotifyDeathStateChange.h>

void NotifyDeathStateChange::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    Serialization::WriteVarInt(aWriter, OwnershipEpoch);
    Serialization::WriteBool(aWriter, IsDead);
}

void NotifyDeathStateChange::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    OwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    IsDead = Serialization::ReadBool(aReader);
}
