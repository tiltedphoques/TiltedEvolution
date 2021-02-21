#pragma once

#include <TiltedCore/Stl.hpp>
#include <Messages/Message.h>

using TiltedPhoques::UniquePtr;

struct ServerMessageFactory
{
    UniquePtr<ServerMessage> Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept;
};
