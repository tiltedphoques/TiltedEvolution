#pragma once

#include "Message.h"

using TiltedPhoques::Vector;

/**
 * @brief Tells a client to delete its copy of dropped items, because they were picked up,
 * evicted, or went out of the client's range.
 */
struct NotifyDroppedItemsRemove final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDroppedItemsRemove;

    NotifyDroppedItemsRemove()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDroppedItemsRemove& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && ServerIds == acRhs.ServerIds; }

    Vector<uint32_t> ServerIds{};
};
