#include <Messages/NotifyChatMessageBroadcast.h>

void NotifyChatMessageBroadcast::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteString(aWriter, PlayerName);
    Serialization::WriteString(aWriter, ChatMessage);
}

void NotifyChatMessageBroadcast::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PlayerName = Serialization::ReadString(aReader);
    ChatMessage = Serialization::ReadString(aReader);
}
