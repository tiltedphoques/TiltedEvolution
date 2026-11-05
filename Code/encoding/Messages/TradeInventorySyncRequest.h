#pragma once

#include "Message.h"
#include <Structs/Inventory.h>

// The trading player's live inventory. Incremental inventory events miss changes made in place,
// such as enchanting, renaming or brewing, so trades work from this snapshot.
struct TradeInventorySyncRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTradeInventorySyncRequest;

    TradeInventorySyncRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    Vector<Inventory::Entry> Entries;
};
