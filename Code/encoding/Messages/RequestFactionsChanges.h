#pragma once

#include "Message.h"
#include <Structs/Factions.h>

using TiltedPhoques::Map;

struct RequestFactionsChanges final : ClientMessage
{
    RequestFactionsChanges()
        : ClientMessage(kRequestFactionsChanges)
    {
    }

    virtual ~RequestFactionsChanges() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestFactionsChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    Map<uint32_t, Factions> Changes;
};
