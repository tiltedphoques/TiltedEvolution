#include <Messages/RequestOwnershipClaim.h>

void RequestOwnershipClaim::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
}

void RequestOwnershipClaim::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
