#include <Messages/NotifyTradeComplete.h>

void NotifyTradeComplete::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PartnerPlayerId);
}

void NotifyTradeComplete::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PartnerPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
