#pragma once

#include "Message.h"
#include <Structs/ReferenceUpdate.h>

using TiltedPhoques::String;

struct ClientReferencesMoveRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kClientReferencesMoveRequest;

    ClientReferencesMoveRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~ClientReferencesMoveRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ClientReferencesMoveRequest& acRhs) const noexcept
    {
        return Updates == acRhs.Updates &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    uint64_t Tick{};
    TiltedPhoques::Map<uint32_t, ReferenceUpdate> Updates{};
};
