#include <Messages/NotifyTeleport.h>
#include <TiltedCore/Serialization.hpp>

void NotifyTeleport::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
    WorldSpaceId.Serialize(aWriter);
}

void NotifyTeleport::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
    WorldSpaceId.Deserialize(aReader);
}
