#include <Messages/PartyAutoJoinToggleRequest.h>

void PartyAutoJoinToggleRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void PartyAutoJoinToggleRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
}
