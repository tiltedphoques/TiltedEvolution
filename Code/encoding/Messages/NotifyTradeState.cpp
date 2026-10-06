#include <Messages/NotifyTradeState.h>

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
}

void NotifyTradeState::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PartnerPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SelfReady = Serialization::ReadBool(aReader);
    PartnerReady = Serialization::ReadBool(aReader);
    CountdownMs = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    CountdownTotalMs = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    auto selfCount = Serialization::ReadVarInt(aReader);
    SelfItems.reserve(selfCount);
    for (size_t i = 0; i < selfCount; ++i)
    {
        Inventory::Entry entry;
        entry.Deserialize(aReader);
        SelfItems.push_back(entry);
    }

    auto partnerCount = Serialization::ReadVarInt(aReader);
    PartnerItems.reserve(partnerCount);
    for (size_t i = 0; i < partnerCount; ++i)
    {
        Inventory::Entry entry;
        entry.Deserialize(aReader);
        PartnerItems.push_back(entry);
    }

    auto inventoryCount = Serialization::ReadVarInt(aReader);
    SelfInventory.reserve(inventoryCount);
    for (size_t i = 0; i < inventoryCount; ++i)
    {
        Inventory::Entry entry;
        entry.Deserialize(aReader);
        SelfInventory.push_back(entry);
    }
}
