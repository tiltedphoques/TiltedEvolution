#pragma once

#include "Message.h"

/**
 * Sent when the number of players in the sleep/wait menu changes. Once all are in, the shared time
 * advances by Hours, the shortest time anyone asked for.
 */
struct NotifyTimeSkip final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyTimeSkip;

    NotifyTimeSkip()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyTimeSkip& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() && Hours == acRhs.Hours && ReadyCount == acRhs.ReadyCount && PlayerCount == acRhs.PlayerCount;
    }

    float Hours{};
    uint32_t ReadyCount{};
    uint32_t PlayerCount{};
};
