
#include <Messages/ServerTimeSettings.h>

void ServerTimeSettings::SerializeRaw(TiltedPhoques::Buffer::Writer &aWriter) const noexcept
{
    DateTime.Serialize(aWriter);
}

void ServerTimeSettings::DeserializeRaw(TiltedPhoques::Buffer::Reader &aReader) noexcept
{
    DateTime.Deserialize(aReader);
}
