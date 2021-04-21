#pragma once

#include "Message.h"

struct RemoveCharacterRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRemoveCharacterRequest;

    RemoveCharacterRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~RemoveCharacterRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RemoveCharacterRequest& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId;
};
