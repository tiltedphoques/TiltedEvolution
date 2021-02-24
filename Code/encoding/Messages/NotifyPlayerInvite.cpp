#include <Messages/NotifyPartyInvite.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPartyInvite::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, InviterId);
    Serialization::WriteVarInt(aWriter, ExpiryTick);
}

void NotifyPartyInvite::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    InviterId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ExpiryTick = Serialization::ReadVarInt(aReader);
}
