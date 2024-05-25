#include <Messages/RequestRespawn.h>

void RequestRespawn::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ActorId);
    Serialization::WriteString(aWriter, AppearanceBuffer);
    Serialization::WriteVarInt(aWriter, ChangeFlags);
}

void RequestRespawn::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ActorId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    AppearanceBuffer = Serialization::ReadString(aReader);
    ChangeFlags = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
