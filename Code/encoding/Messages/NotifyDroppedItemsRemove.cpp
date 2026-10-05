#include <Messages/NotifyDroppedItemsRemove.h>
#include <TiltedCore/Serialization.hpp>

void NotifyDroppedItemsRemove::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerIds.size());

    for (const uint32_t serverId : ServerIds)
        Serialization::WriteVarInt(aWriter, serverId);
}

void NotifyDroppedItemsRemove::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    const auto count = Serialization::ReadVarInt(aReader);
    ServerIds.resize(count);

    for (auto& serverId : ServerIds)
        serverId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
