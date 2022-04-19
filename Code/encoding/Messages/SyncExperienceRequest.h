#pragma once

#include "Message.h"

struct SyncExperienceRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kSyncExperienceRequest;

    SyncExperienceRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const SyncExperienceRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               Experience == acRhs.Experience;
    }

    float Experience{};
};
