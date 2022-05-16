#include <Messages/NotifyPlayerRespawn.h>

void NotifyPlayerRespawn::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, GoldLost);
}

void NotifyPlayerRespawn::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    GoldLost = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
