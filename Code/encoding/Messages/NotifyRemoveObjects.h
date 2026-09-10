#pragma once

#include "Message.h"

struct NotifyRemoveObjects final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyRemoveObjects;

    NotifyRemoveObjects()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyRemoveObjects& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && ServerIds == acRhs.ServerIds; }

    TiltedPhoques::Vector<uint32_t> ServerIds{};
};
