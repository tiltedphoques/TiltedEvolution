#include <Messages/TradeSetReadyRequest.h>

void TradeSetReadyRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Ready);
}

void TradeSetReadyRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Ready = Serialization::ReadBool(aReader);
}
