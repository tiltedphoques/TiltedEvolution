#pragma once

#include "Message.h"
#include <Buffer.hpp>

struct CancelAssignmentRequest final : ClientMessage
{
    CancelAssignmentRequest()
        : ClientMessage(kAuthenticationRequest)
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
