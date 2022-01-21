#include <Messages/PartyChangeLeaderRequest.h>

void PartyChangeLeaderRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PartyMemberPlayerId);
}

void PartyChangeLeaderRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{ 
    ClientMessage::DeserializeRaw(aReader);
    PartyMemberPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
