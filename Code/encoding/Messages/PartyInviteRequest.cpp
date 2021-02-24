#include <Messages/PartyInviteRequest.h>

void PartyInviteRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PlayerId);
}

void PartyInviteRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    PlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
