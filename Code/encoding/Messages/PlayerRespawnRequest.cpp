#include <Messages/PlayerRespawnRequest.h>

void PlayerRespawnRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void PlayerRespawnRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
}
