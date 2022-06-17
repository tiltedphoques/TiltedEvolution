#include <Messages/RequestDelWaypoint.h>

void RequestDelWaypoint::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void RequestDelWaypoint::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
}
