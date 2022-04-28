#pragma once

#include "Message.h"

struct StringCacheUpdate final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kStringCacheUpdate;

    StringCacheUpdate() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const StringCacheUpdate& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && StartId == achRhs.StartId && Values == achRhs.Values;
    }

    uint16_t StartId{0};
    TiltedPhoques::Vector<String> Values;
};
