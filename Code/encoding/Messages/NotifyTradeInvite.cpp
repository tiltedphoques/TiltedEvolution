#include <Messages/NotifyTradeInvite.h>

void NotifyTradeInvite::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, InviterPlayerId);
    Serialization::WriteVarInt(aWriter, ExpiryTick);
}

void NotifyTradeInvite::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    InviterPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ExpiryTick = Serialization::ReadVarInt(aReader);
}
