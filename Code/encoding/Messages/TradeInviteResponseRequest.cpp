#include <Messages/TradeInviteResponseRequest.h>

void TradeInviteResponseRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, RequesterPlayerId);
    Serialization::WriteBool(aWriter, Accept);
}

void TradeInviteResponseRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    RequesterPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Accept = Serialization::ReadBool(aReader);
}
