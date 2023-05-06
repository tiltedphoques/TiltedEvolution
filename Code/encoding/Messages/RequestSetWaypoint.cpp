#include <Messages/RequestSetWaypoint.h>

void RequestSetWaypoint::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Position.Serialize(aWriter);
}

void RequestSetWaypoint::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Position.Deserialize(aReader);
}
