#include <Messages/SendChatMessageRequest.h>

void SendChatMessageRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, static_cast<uint64_t>(MessageType));
    Serialization::WriteString(aWriter, ChatMessage);
}

void SendChatMessageRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    MessageType = static_cast<ChatMessageType>(Serialization::ReadVarInt(aReader));
    ChatMessage = Serialization::ReadString(aReader);
}
