#include <Messages/NotifyRemoveObjects.h>

void NotifyRemoveObjects::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerIds.size());
    for (const uint32_t serverId : ServerIds)
        Serialization::WriteVarInt(aWriter, serverId);
}

void NotifyRemoveObjects::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    const auto count = Serialization::ReadVarInt(aReader);
    ServerIds.resize(count);
    for (auto& serverId : ServerIds)
        serverId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
