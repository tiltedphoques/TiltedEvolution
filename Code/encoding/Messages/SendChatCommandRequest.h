#pragma once

#include "Message.h"

using TiltedPhoques::String;

struct SendChatCommandRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kSendChatCommandRequest;

    SendChatCommandRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~SendChatCommandRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const SendChatCommandRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ChatMessage == acRhs.ChatMessage;
    }

    TiltedPhoques::String ChatMessage;
};

