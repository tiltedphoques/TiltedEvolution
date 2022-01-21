#include <Messages/NotifyPartyLeft.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPartyLeft::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void NotifyPartyLeft::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);
}
