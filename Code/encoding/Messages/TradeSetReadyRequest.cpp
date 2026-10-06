#include <Messages/TradeSetReadyRequest.h>

void TradeSetReadyRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Ready);
    Serialization::WriteVarInt(aWriter, PreparedPotions.size());
    for (const auto& item : PreparedPotions)
        item.Serialize(aWriter);
}

void TradeSetReadyRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Ready = Serialization::ReadBool(aReader);
    PreparedPotions.clear();
    const auto count = Serialization::ReadVarInt(aReader);
    for (uint64_t i = 0; i < count; ++i)
    {
        Inventory::Entry item;
        item.Deserialize(aReader);
        PreparedPotions.push_back(std::move(item));
    }
}
