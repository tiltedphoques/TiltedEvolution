#include <Messages/TradeOfferUpdateRequest.h>

void TradeOfferUpdateRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Items.size());
    for (const auto& item : Items)
        item.Serialize(aWriter);
}

void TradeOfferUpdateRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    const auto count = Serialization::ReadVarInt(aReader);
    Items.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        Inventory::Entry entry;
        entry.Deserialize(aReader);
        Items.push_back(entry);
    }
}
