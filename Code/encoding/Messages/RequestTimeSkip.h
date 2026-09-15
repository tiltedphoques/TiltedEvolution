#pragma once

#include "Message.h"

/**
 * Sent when the local player finished sleeping or waiting, so that the server can advance the shared
 * game time by the same amount for everyone (#467).
 */
struct RequestTimeSkip final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestTimeSkip;

    RequestTimeSkip()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestTimeSkip& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && Hours == acRhs.Hours; }

    // Game hours to skip, as requested in the sleep/wait menu.
    float Hours{};
};
