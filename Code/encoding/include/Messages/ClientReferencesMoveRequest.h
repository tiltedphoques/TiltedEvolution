#pragma once

#include "Message.h"
#include <Structs/ReferenceUpdate.h>
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Map;

struct ClientReferencesMoveRequest final : ClientMessage
{
    ClientReferencesMoveRequest()
        : ClientMessage(kClientReferencesMoveRequest)
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
    Map<uint32_t, ReferenceUpdate> Updates{};
};
