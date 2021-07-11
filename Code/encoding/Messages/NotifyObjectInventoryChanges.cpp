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
        GameId gameId;
        gameId.Deserialize(aReader);

        auto& change = Changes[gameId];
        change.Deserialize(aReader);
    }
}
