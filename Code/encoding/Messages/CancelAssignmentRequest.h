#pragma once

#include "Message.h"

struct CancelAssignmentRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kCancelAssignmentRequest;

    CancelAssignmentRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~CancelAssignmentRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const CancelAssignmentRequest& achRhs) const noexcept
    {
        return Cookie == achRhs.Cookie &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t Cookie;
};
