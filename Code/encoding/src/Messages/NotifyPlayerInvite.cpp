#include <Messages/NotifyPartyInvite.h>
#include <Serialization.hpp>

void NotifyPartyInvite::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, InviterId);
}

void NotifyPartyInvite::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    InviterId = Serialization::ReadVarInt(aReader);
}
