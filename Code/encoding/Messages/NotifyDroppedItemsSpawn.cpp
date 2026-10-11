#include <Messages/NotifyDroppedItemsSpawn.h>
#include <TiltedCore/Serialization.hpp>

void NotifyDroppedItemsSpawn::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Items.size());

    for (const auto& item : Items)
        item.Serialize(aWriter);
}

void NotifyDroppedItemsSpawn::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    const auto count = Serialization::ReadVarInt(aReader);
    Items.resize(count);

    for (auto& item : Items)
        item.Deserialize(aReader);
}
