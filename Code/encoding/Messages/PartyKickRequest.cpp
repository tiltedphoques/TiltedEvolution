#include <Messages/PartyKickRequest.h>

void PartyKickRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PartyMemberPlayerId);
}

void PartyKickRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
    PartyMemberPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
