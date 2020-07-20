#pragma once

#include "Message.h"
#include <Structs/GameId.h>
#include <Structs/ActionEvent.h>
#include <Structs/Movement.h>
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Map;

struct ServerReferencesMoveRequest final : ServerMessage
{
    ServerReferencesMoveRequest()
        : ServerMessage(kServerReferencesMoveRequest)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ServerReferencesMoveRequest& acRhs) const noexcept
    {
        return Movements == acRhs.Movements &&
            Tick == acRhs.Tick &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    uint64_t Tick{};
    Map<uint32_t, Movement> Movements{};
};
