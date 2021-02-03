#include <Messages/ClientRpcCalls.h>

void ClientRpcCalls::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteString(aWriter, Data);
}

void ClientRpcCalls::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Data = Serialization::ReadString(aReader);
}
