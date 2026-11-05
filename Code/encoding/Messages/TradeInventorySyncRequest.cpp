#include <Messages/TradeInventorySyncRequest.h>

#include <Decoding.h>

void TradeInventorySyncRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Entries.size());
    for (const auto& entry : Entries)
        entry.Serialize(aWriter);
}

void TradeInventorySyncRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Decoding::ReadList(aReader, Entries, Inventory::kMaxEntries);
}
