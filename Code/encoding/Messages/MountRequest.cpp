#include <Messages/MountRequest.h>

void MountRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, RiderId);
    Serialization::WriteVarInt(aWriter, MountId);
}

void MountRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    RiderId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    MountId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
