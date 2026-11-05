#include <Messages/TradeOfferUpdateRequest.h>

#include <Decoding.h>

void TradeOfferUpdateRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Items.size());
    for (const auto& item : Items)
        item.Serialize(aWriter);
}

void TradeOfferUpdateRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Decoding::ReadList(aReader, Items, Inventory::kMaxEntries);
}
