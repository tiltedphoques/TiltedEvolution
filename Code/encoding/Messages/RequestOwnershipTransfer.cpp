#include <Messages/RequestOwnershipTransfer.h>

void RequestOwnershipTransfer::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteVarInt(aWriter, OwnershipEpoch);
    aWriter.WriteBits(static_cast<uint8_t>(Reason), 8);
    WorldSpaceId.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
}

void RequestOwnershipTransfer::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    OwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    uint64_t reason{};
    aReader.ReadBits(reason, 8);
    Reason = static_cast<OwnershipReleaseReason>(reason);
    WorldSpaceId.Deserialize(aReader);
    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
}
