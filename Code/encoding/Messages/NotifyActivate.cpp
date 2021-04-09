#include <Messages/NotifyActivate.h>

void NotifyActivate::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    Serialization::WriteVarInt(aWriter, ActivatorId);
}

void NotifyActivate::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ActivatorId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
