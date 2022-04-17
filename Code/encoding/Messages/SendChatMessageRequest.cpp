#include <Messages/SendChatMessageRequest.h>

void SendChatMessageRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteString(aWriter, ChatMessage);
}

void SendChatMessageRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ChatMessage = Serialization::ReadString(aReader);
}
