#include <Messages/TopicRequest.h>
#include <TiltedCore/Serialization.hpp>

void TopicRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, SpeakerID);
    Serialization::WriteVarInt(aWriter, Type);
    TopicID1.Serialize(aWriter);
    TopicID2.Serialize(aWriter);
}

void TopicRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    SpeakerID = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Type = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    TopicID1.Deserialize(aReader);
    TopicID2.Deserialize(aReader);
}
