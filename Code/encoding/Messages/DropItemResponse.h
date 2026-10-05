#pragma once

#include "Message.h"

/**
 * @brief Tells the dropping client which server id was assigned to its dropped item.
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

    bool operator==(const DropItemResponse& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && LocalId == acRhs.LocalId && ServerId == acRhs.ServerId; }

    uint32_t LocalId{};
    uint32_t ServerId{};
};
