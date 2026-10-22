#include <Messages/TradeInviteRequest.h>

void TradeInviteRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, TargetPlayerId);
}

void TradeInviteRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    TargetPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
