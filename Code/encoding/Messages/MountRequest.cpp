#include <Messages/MountRequest.h>

void MountRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, RiderId);
    Serialization::WriteVarInt(aWriter, RiderOwnershipEpoch);
    Serialization::WriteVarInt(aWriter, MountId);
    Serialization::WriteVarInt(aWriter, MountOwnershipEpoch);
}

void MountRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    RiderId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    RiderOwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    MountId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    MountOwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
