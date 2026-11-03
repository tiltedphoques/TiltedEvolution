#pragma once

#include "Message.h"
#include <Structs/Movement.h>

using TiltedPhoques::String;

/**
 * @brief Movement snapshot relayed as soon as the owner's ClientReferencesMoveRequest arrives. Sent unreliably.
 */
struct ServerReferencesMoveRequest final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kServerReferencesMoveRequest;

    ServerReferencesMoveRequest()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ServerReferencesMoveRequest& acRhs) const noexcept { return Updates == acRhs.Updates && Tick == acRhs.Tick && GetOpcode() == acRhs.GetOpcode(); }

    // The owner's capture tick, forwarded untouched
    uint64_t Tick{};
    TiltedPhoques::Map<uint32_t, Movement> Updates{};
};
