#pragma once

#include "Message.h"

/**
 * Sent when the local player confirms the sleep/wait menu, or with 0 hours when they leave it.
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

    float Hours{};
};
