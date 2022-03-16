#pragma once

#include "Message.h"
#include <Structs/Factions.h>

struct RequestFactionsChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestFactionsChanges;

    RequestFactionsChanges() : ClientMessage(Opcode)
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
    
    TiltedPhoques::Map<uint32_t, Factions> Changes;
};
