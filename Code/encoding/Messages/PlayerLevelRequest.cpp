#include <Messages/PlayerLevelRequest.h>

void PlayerLevelRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, NewLevel);
}

void PlayerLevelRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    NewLevel = Serialization::ReadVarInt(aReader) & 0xFFFF;
}
