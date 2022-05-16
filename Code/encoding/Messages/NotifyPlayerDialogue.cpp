#include <Messages/NotifyPlayerDialogue.h>

void NotifyPlayerDialogue::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteString(aWriter, Text);
}

void NotifyPlayerDialogue::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Text = Serialization::ReadString(aReader);
}
