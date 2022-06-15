#pragma once

#include "Message.h"
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/Rotator2_NetQuantize.h>

struct NotifyPlayerPosition final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyPlayerPosition;

    NotifyPlayerPosition() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyPlayerPosition& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               PlayerId == acRhs.PlayerId &&
               Position == acRhs.Position &&
               Rotation == acRhs.Rotation;
    }

    uint32_t PlayerId{};
    Vector3_NetQuantize Position{};
    Rotator2_NetQuantize Rotation{};
};
