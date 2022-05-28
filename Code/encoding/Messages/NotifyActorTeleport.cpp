#include <Messages/NotifyActorTeleport.h>

void NotifyActorTeleport::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    FormId.Serialize(aWriter);
    WorldSpaceId.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
}

void NotifyActorTeleport::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    FormId.Deserialize(aReader);
    WorldSpaceId.Deserialize(aReader);
    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
}
