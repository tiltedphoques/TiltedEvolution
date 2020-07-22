#pragma once

#include "Message.h"
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Map;

struct NotifyInventoryChanges final : ServerMessage
{
    NotifyInventoryChanges()
        : ServerMessage(kNotifyInventoryChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyInventoryChanges& acRhs) const noexcept
    {
        return Changes == acRhs.Changes &&
            GetOpcode() == acRhs.GetOpcode();
    }
    
    Map<uint32_t, String> Changes{};
};
