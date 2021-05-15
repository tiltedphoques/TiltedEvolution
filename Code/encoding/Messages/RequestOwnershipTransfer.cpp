#include <Messages/RequestOwnershipTransfer.h>

void RequestOwnershipTransfer::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
}

void RequestOwnershipTransfer::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
