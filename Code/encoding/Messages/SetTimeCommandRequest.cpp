#include <Messages/SetTimeCommandRequest.h>

void SetTimeCommandRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Hours);
    Serialization::WriteVarInt(aWriter, Minutes);
}

void SetTimeCommandRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Hours = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Minutes = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
