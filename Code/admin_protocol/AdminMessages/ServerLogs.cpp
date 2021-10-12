#include "ServerLogs.h"


void ServerLogs::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteString(aWriter, Logs);
}

void ServerLogs::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Logs = Serialization::ReadString(aReader);
}
