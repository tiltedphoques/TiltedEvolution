#pragma once

#include "Message.h"
#include <Structs/ReferenceUpdate.h>
#include <TiltedCore/Stl.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Map;

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
    Map<uint32_t, ReferenceUpdate> Updates{};
};
