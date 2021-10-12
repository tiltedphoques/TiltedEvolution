#include <Messages/AttachArrowRequest.h>

void AttachArrowRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ShooterId);
}

void AttachArrowRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ShooterId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
