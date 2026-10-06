#pragma once

#include "Message.h"

#include <Structs/Inventory.h>

struct NotifyTradeState final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTradeState;

    NotifyTradeState()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    uint32_t PartnerPlayerId{};
    bool SelfReady{false};
    bool PartnerReady{false};
    uint32_t CountdownMs{0};
    uint32_t CountdownTotalMs{0};
    TiltedPhoques::Vector<Inventory::Entry> SelfItems;
    TiltedPhoques::Vector<Inventory::Entry> PartnerItems;
    TiltedPhoques::Vector<Inventory::Entry> SelfInventory;
};
