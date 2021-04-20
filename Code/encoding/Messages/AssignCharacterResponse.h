#pragma once

#include "Message.h"
#include <Structs/Mods.h>
#include <Structs/ActorValues.h>

struct AssignCharacterResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kAssignCharacterResponse;

    AssignCharacterResponse() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AssignCharacterResponse& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
            Owner == achRhs.Owner &&
            Cookie == achRhs.Cookie &&
            ServerId == achRhs.ServerId &&
            AllActorValues == achRhs.AllActorValues;
    }

    bool Owner{ false };
    uint32_t Cookie{};
    uint32_t ServerId{};
    ActorValues AllActorValues{};
};
