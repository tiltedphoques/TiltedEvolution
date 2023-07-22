
#include <Messages/ServerTimeSettings.h>

void ServerTimeSettings::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept { timeModel.Serialize(aWriter); }

void ServerTimeSettings::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept { timeModel.Deserialize(aReader); }

bool ServerTimeSettings::operator==(const ServerTimeSettings& achRhs) const noexcept { return GetOpcode() == achRhs.GetOpcode() && timeModel == achRhs.timeModel; }

bool ServerTimeSettings::operator!=(const ServerTimeSettings& achRhs) const noexcept { return !this->operator==(achRhs); }
