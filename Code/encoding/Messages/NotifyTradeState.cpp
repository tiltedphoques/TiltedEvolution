#include <Messages/NotifyTradeState.h>

#include <Decoding.h>

void NotifyTradeState::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PartnerPlayerId);
    Serialization::WriteBool(aWriter, SelfReady);
    Serialization::WriteBool(aWriter, PartnerReady);
    Serialization::WriteVarInt(aWriter, CountdownMs);
    Serialization::WriteVarInt(aWriter, CountdownTotalMs);

    Serialization::WriteVarInt(aWriter, SelfItems.size());
    for (const auto& item : SelfItems)
        item.Serialize(aWriter);

    Serialization::WriteVarInt(aWriter, PartnerItems.size());
    for (const auto& item : PartnerItems)
        item.Serialize(aWriter);

    Serialization::WriteVarInt(aWriter, SelfInventory.size());
    for (const auto& item : SelfInventory)
        item.Serialize(aWriter);

    Serialization::WriteVarInt(aWriter, SelfAcceptedItems.size());
    for (const auto& item : SelfAcceptedItems)
        item.Serialize(aWriter);
}

void NotifyTradeState::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PartnerPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SelfReady = Serialization::ReadBool(aReader);
    PartnerReady = Serialization::ReadBool(aReader);
    CountdownMs = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CountdownTotalMs = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    Decoding::ReadList(aReader, SelfItems, Inventory::kMaxEntries);
    Decoding::ReadList(aReader, PartnerItems, Inventory::kMaxEntries);
    Decoding::ReadList(aReader, SelfInventory, Inventory::kMaxEntries);
    Decoding::ReadList(aReader, SelfAcceptedItems, Inventory::kMaxEntries);
}
