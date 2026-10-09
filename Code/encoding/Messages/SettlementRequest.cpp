#include <Messages/SettlementRequest.h>

void SettlementRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(static_cast<uint8_t>(Operation), 8);
    TiltedPhoques::Serialization::WriteVarInt(aWriter, SessionId);
    TiltedPhoques::Serialization::WriteVarInt(aWriter, RequestId);
    Data.Serialize(aWriter);
}

void SettlementRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
    uint64_t operation{};
    aReader.ReadBits(operation, 8);
    Operation = static_cast<Action>(operation);
    SessionId = TiltedPhoques::Serialization::ReadVarInt(aReader);
    RequestId = TiltedPhoques::Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Data.Deserialize(aReader);
}
