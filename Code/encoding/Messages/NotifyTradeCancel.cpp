#include <Messages/NotifyTradeCancel.h>

void NotifyTradeCancel::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PartnerPlayerId);
    Serialization::WriteVarInt(aWriter, static_cast<uint64_t>(Reason));
    Serialization::WriteBool(aWriter, WasInitiator);
}

void NotifyTradeCancel::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PartnerPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Reason = static_cast<TradeCancelReason>(Serialization::ReadVarInt(aReader));
    WasInitiator = Serialization::ReadBool(aReader);
}
