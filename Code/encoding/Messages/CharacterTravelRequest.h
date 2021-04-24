#pragma once

#include "Message.h"
#include "Structs/GameId.h"
#include "Structs/Vector3_NetQuantize.h"

struct CharacterTravelRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kCharacterTravelRequest;

    CharacterTravelRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~CharacterTravelRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const CharacterTravelRequest& achRhs) const noexcept
    {
        return ServerId == achRhs.ServerId &&
            CellId == achRhs.CellId && 
            Position == achRhs.Position &&
            GetOpcode() == achRhs.GetOpcode();
    }

    uint32_t ServerId;
    GameId CellId;
    Vector3_NetQuantize Position;
};
