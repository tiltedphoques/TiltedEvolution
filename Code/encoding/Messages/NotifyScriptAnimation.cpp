#include <Messages/NotifyScriptAnimation.h>

void NotifyScriptAnimation::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, FormID);
    Serialization::WriteString(aWriter, Animation);
    Serialization::WriteString(aWriter, EventName);
}

void NotifyScriptAnimation::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    FormID = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Animation = Serialization::ReadString(aReader);
    EventName = Serialization::ReadString(aReader);
}
