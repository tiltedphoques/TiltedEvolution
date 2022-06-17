#include <Messages/NotifyDelWaypoint.h>

void NotifyDelWaypoint::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void NotifyDelWaypoint::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);
}
