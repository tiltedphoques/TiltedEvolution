
#include <Messages/ServerTimeSettings.h>

void ServerTimeSettings::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept { TimeModel.Serialize(aWriter); }

void ServerTimeSettings::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept { TimeModel.Deserialize(aReader); }

bool ServerTimeSettings::operator==(const ServerTimeSettings& achRhs) const noexcept { return GetOpcode() == achRhs.GetOpcode() && TimeModel == achRhs.TimeModel; }

bool ServerTimeSettings::operator!=(const ServerTimeSettings& achRhs) const noexcept { return !this->operator==(achRhs); }
