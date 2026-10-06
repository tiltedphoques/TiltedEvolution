#include <Messages/NotifyTradeStarted.h>

void NotifyTradeStarted::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PartnerPlayerId);
    Serialization::WriteBool(aWriter, InitiatedBySelf);
}

void NotifyTradeStarted::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PartnerPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    InitiatedBySelf = Serialization::ReadBool(aReader);
}
