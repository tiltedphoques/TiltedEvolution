#include <Messages/NotifySyncExperience.h>

void NotifySyncExperience::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, Experience);
}

void NotifySyncExperience::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Experience = Serialization::ReadFloat(aReader);
}
