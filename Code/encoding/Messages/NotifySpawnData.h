#pragma once

#include "Message.h"
#include <Structs/ActorValues.h>
#include <Structs/Inventory.h>

struct NotifySpawnData final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySpawnData;

    NotifySpawnData() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifySpawnData& acRhs) const noexcept
    {
        return Id == acRhs.Id &&
               InitialActorValues == acRhs.InitialActorValues && 
               InitialInventory == acRhs.InitialInventory &&
               IsDead == acRhs.IsDead &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    ActorValues InitialActorValues{};
    Inventory InitialInventory{};
    bool IsDead{};
};
