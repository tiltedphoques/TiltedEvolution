#include <Messages/TradeInventorySyncRequest.h>

void TradeInventorySyncRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Entries.size());
    for (const auto& entry : Entries)
        entry.Serialize(aWriter);
}

void TradeInventorySyncRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Entries.clear();
    const auto count = Serialization::ReadVarInt(aReader);
    for (uint64_t i = 0; i < count; ++i)
    {
        Inventory::Entry entry;
        entry.Deserialize(aReader);
        Entries.push_back(std::move(entry));
    }
}
