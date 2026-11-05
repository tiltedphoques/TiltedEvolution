#include <Messages/TradeSetReadyRequest.h>

#include <Decoding.h>

void TradeSetReadyRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Ready);
    Serialization::WriteVarInt(aWriter, AcceptedItems.size());
    for (const auto& item : AcceptedItems)
        item.Serialize(aWriter);
}

void TradeSetReadyRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Ready = Serialization::ReadBool(aReader);
    Decoding::ReadList(aReader, AcceptedItems, Inventory::kMaxEntries);
}
