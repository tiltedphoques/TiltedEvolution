#include <Messages/TradeSetReadyRequest.h>

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
    AcceptedItems.clear();
    const auto count = Serialization::ReadVarInt(aReader);
    for (uint64_t i = 0; i < count; ++i)
    {
        Inventory::Entry item;
        item.Deserialize(aReader);
        AcceptedItems.push_back(std::move(item));
    }
}
