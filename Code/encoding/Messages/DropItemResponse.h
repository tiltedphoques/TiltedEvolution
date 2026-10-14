#pragma once

#include "Message.h"

/**
 * @brief Assigns a server id to an item the receiving client dropped.
 */
struct DropItemResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kDropItemResponse;

    DropItemResponse()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const DropItemResponse& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && RefId == acRhs.RefId && ServerId == acRhs.ServerId; }

    uint32_t RefId{};
    // 0 if the server does not track dropped items.
    uint32_t ServerId{};
};
