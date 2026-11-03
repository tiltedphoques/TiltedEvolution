#pragma once

#include "Message.h"
#include <Structs/Movement.h>

using TiltedPhoques::String;

/**
 * @brief Movement snapshot of actors owned by the sender. Sent unreliably, so it can be lost or arrive out of order.
 */
struct ClientReferencesMoveRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kClientReferencesMoveRequest;

    ClientReferencesMoveRequest()
        : ClientMessage(Opcode)
    {
    }

    virtual ~ClientReferencesMoveRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ClientReferencesMoveRequest& acRhs) const noexcept { return Updates == acRhs.Updates && Tick == acRhs.Tick && GetOpcode() == acRhs.GetOpcode(); }

    // Synchronized clock tick at which the owner captured the snapshot
    uint64_t Tick{};
    TiltedPhoques::Map<uint32_t, Movement> Updates{};
};
