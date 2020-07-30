#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Structs/Factions.h>

using TiltedPhoques::Map;

struct NotifyFactionsChanges final : ServerMessage
{
    NotifyFactionsChanges()
        : ServerMessage(kNotifyFactionsChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyFactionsChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    Map<uint32_t, Factions> Changes{};
};
