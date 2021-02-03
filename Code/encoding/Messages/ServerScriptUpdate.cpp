#include <Messages/ServerScriptUpdate.h>

void ServerScriptUpdate::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Data.Serialize(aWriter);
}

void ServerScriptUpdate::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Data.Deserialize(aReader);
}
