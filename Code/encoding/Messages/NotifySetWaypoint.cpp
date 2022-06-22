#include <Messages/NotifySetWaypoint.h>

void NotifySetWaypoint::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Position.Serialize(aWriter);
}

void NotifySetWaypoint::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Position.Deserialize(aReader);
}
