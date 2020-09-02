#pragma once

#include "Message.h"
#include <Stl.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct NotifyPartyInfo final : ServerMessage
{
    NotifyPartyInfo() : 
        ServerMessage(kNotifyPartyInfo)
    {
    }

    virtual ~NotifyPartyInfo() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPartyInfo& acRhs) const noexcept
    {
        return PlayerIds == acRhs.PlayerIds &&
            GetOpcode() == acRhs.GetOpcode();
    }

    Vector<uint32_t> PlayerIds{};
};
