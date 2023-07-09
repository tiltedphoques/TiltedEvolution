#include <Messages/NotifyDeleteWaypoint.h>

void NotifyDeleteWaypoint::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void NotifyDeleteWaypoint::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);
}
