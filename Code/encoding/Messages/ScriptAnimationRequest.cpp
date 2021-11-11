#include <Messages/ScriptAnimationRequest.h>

void ScriptAnimationRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, FormID);
    Serialization::WriteString(aWriter, Animation);
    Serialization::WriteString(aWriter, EventName);
}

void ScriptAnimationRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    FormID = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Animation = Serialization::ReadString(aReader);
    EventName = Serialization::ReadString(aReader);
}
