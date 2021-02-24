#include <Messages/PartyAcceptInviteRequest.h>

void PartyAcceptInviteRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, InviterId);
}

void PartyAcceptInviteRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    InviterId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
