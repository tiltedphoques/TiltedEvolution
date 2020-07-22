#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Map;

struct RequestInventoryChanges final : ClientMessage
{
    RequestInventoryChanges()
        : ClientMessage(kRequestInventoryChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestInventoryChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    Map<uint32_t, String> Changes;
};
