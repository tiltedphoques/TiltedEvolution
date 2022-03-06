#include <Messages/PartyCreateRequest.h>

void PartyCreateRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void PartyCreateRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
}
