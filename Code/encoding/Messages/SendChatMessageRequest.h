#pragma once

#include "Message.h"

using TiltedPhoques::String;

struct SendChatMessageRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kSendChatMessageRequest;

    SendChatMessageRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~SendChatMessageRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const SendChatMessageRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ChatMessage == acRhs.ChatMessage;
    }

    TiltedPhoques::String ChatMessage;
};
