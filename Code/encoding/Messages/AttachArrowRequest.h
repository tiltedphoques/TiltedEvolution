#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct AttachArrowRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kAttachArrowRequest;

    AttachArrowRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AttachArrowRequest& acRhs) const noexcept
    {
        return ShooterId == acRhs.ShooterId && 
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t ShooterId;
};
