#include <Messages/NotifySettlement.h>

void NotifySettlement::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    using TiltedPhoques::Serialization;
    Serialization::WriteVarInt(aWriter, SessionId);
    Serialization::WriteVarInt(aWriter, LeaderPlayerId);
    Serialization::WriteVarInt(aWriter, SenderId);
    Serialization::WriteVarInt(aWriter, RequestId);
    aWriter.WriteBits(Accepted, 1);
    Data.Serialize(aWriter);
}

void NotifySettlement::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);
    using TiltedPhoques::Serialization;
    SessionId = Serialization::ReadVarInt(aReader);
    LeaderPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SenderId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    RequestId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    uint64_t accepted{};
    aReader.ReadBits(accepted, 1);
    Accepted = accepted != 0;
    Data.Deserialize(aReader);
}
