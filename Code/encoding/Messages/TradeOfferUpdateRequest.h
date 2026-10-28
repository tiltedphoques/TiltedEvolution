#pragma once

#include "Message.h"

#include <Structs/Inventory.h>

struct TradeOfferUpdateRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTradeOfferUpdateRequest;

    TradeOfferUpdateRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    TiltedPhoques::Vector<Inventory::Entry> Items;
};
