#include <Messages/NotifyOwnershipTransfer.h>

void NotifyOwnershipTransfer::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteVarInt(aWriter, OwnerPlayerId);
    Serialization::WriteVarInt(aWriter, OwnershipEpoch);
    CurrentActorData.Serialize(aWriter);
}

void NotifyOwnershipTransfer::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    OwnerPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    OwnershipEpoch = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CurrentActorData.Deserialize(aReader);
}
