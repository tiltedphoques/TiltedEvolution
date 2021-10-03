#include <Messages/NotifyAttachArrow.h>

void NotifyAttachArrow::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ShooterId);
}

void NotifyAttachArrow::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ShooterId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
