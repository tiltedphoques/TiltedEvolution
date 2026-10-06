#pragma once

#include "Message.h"
#include <Structs/Inventory.h>

struct TradeSetReadyRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kTradeSetReadyRequest;

    TradeSetReadyRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool Ready{false};
    Vector<Inventory::Entry> PreparedPotions;
};
