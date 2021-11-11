#include <Messages/InterruptCastRequest.h>

void InterruptCastRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, CasterId);
}

void InterruptCastRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    CasterId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
