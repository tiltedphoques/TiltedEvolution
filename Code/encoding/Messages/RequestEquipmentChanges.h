#pragma once

#include "Message.h"
#include <Structs/Equipment.h>

struct RequestEquipmentChanges final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestEquipmentChanges;

    RequestEquipmentChanges() : ClientMessage(Opcode)
    {
    }

    virtual ~RequestEquipmentChanges() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const RequestEquipmentChanges& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               CurrentEquipment == acRhs.CurrentEquipment;
    }
    
    uint32_t ServerId{};
    Equipment CurrentEquipment{};
};
