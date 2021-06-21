#include <Messages/NotifyObjectInventoryChanges.h>
#include <TiltedCore/Serialization.hpp>

void NotifyObjectInventoryChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Changes.size());

    for (auto& change : Changes)
    {
        change.first.Serialize(aWriter);
        change.second.Serialize(aWriter);
    }
}

void NotifyObjectInventoryChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    auto count = Serialization::ReadVarInt(aReader) & 0xFF;

    for (auto i = 0u; i < count; ++i)
    {
        const uint32_t baseId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
        const uint32_t modId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
        const auto gameId = GameId(modId, baseId);

        auto& change = Changes[gameId];
        change.Deserialize(aReader);
    }
}
