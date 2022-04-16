#include <Messages/TeleportCommandResponse.h>

void TeleportCommandResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    CellId.Serialize(aWriter);
    Position.Serialize(aWriter);
}

void TeleportCommandResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    CellId.Deserialize(aReader);
    Position.Deserialize(aReader);
}
