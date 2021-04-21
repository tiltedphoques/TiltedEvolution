#pragma once

#include "Message.h"
#include <Structs/ReferenceUpdate.h>

using TiltedPhoques::String;
using TiltedPhoques::Map;

struct ServerReferencesMoveRequest final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kServerReferencesMoveRequest;

    ServerReferencesMoveRequest() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ServerReferencesMoveRequest& acRhs) const noexcept
    {
        return Updates == acRhs.Updates &&
            Tick == acRhs.Tick &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    uint64_t Tick{};
    Map<uint32_t, ReferenceUpdate> Updates{};
};
