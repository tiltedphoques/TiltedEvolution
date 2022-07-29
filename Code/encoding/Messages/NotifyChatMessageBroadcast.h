#pragma once

#include "Message.h"

using TiltedPhoques::String;

struct NotifyChatMessageBroadcast final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyChatMessageBroadcast;

    NotifyChatMessageBroadcast() : ServerMessage(Opcode)
    {
    }

    virtual ~NotifyChatMessageBroadcast() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyChatMessageBroadcast& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               MessageType == acRhs.MessageType && 
               PlayerName == acRhs.PlayerName && 
               ChatMessage == acRhs.ChatMessage;
    }

    ChatMessageType MessageType;
    TiltedPhoques::String PlayerName;
    TiltedPhoques::String ChatMessage;
};
