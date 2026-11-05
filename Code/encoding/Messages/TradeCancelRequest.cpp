#include <Messages/TradeCancelRequest.h>

void TradeCancelRequest::SerializeRaw(TiltedPhoques::Buffer::Writer&) const noexcept
{
}

void TradeCancelRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
}
