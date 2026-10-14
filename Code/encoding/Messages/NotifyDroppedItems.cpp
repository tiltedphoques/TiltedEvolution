#include <Messages/NotifyDroppedItems.h>

void NotifyDroppedItems::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Items.size());

    for (const auto& item : Items)
        item.Serialize(aWriter);
}

void NotifyDroppedItems::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    const uint64_t count = Serialization::ReadVarInt(aReader);
    Items.resize(count);

    for (auto& item : Items)
        item.Deserialize(aReader);
}
