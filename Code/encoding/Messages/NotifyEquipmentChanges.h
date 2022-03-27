#pragma once

#include "Message.h"
#include <TiltedCore/Buffer.hpp>
#include <Structs/Equipment.h>

struct NotifyEquipmentChanges final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyEquipmentChanges;

    NotifyEquipmentChanges() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyEquipmentChanges& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               CurrentEquipment == acRhs.CurrentEquipment;
    }
    
    uint32_t ServerId{};
    Equipment CurrentEquipment{};
};
