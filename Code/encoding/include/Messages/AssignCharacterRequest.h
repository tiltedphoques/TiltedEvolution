#pragma once

#include "Message.h"
#include <Structs/GameId.h>
#include <Structs/ActionEvent.h>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/Rotator2_NetQuantize.h>
#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::String;

struct AssignCharacterRequest final : ClientMessage
{
    AssignCharacterRequest()
        : ClientMessage(kAssignCharacterRequest)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AssignCharacterRequest& acRhs) const noexcept
    {
        return Cookie == acRhs.Cookie &&
            ReferenceId == acRhs.ReferenceId &&
            FormId == acRhs.FormId &&
            CellId == acRhs.CellId &&
            Position == acRhs.Position &&
            Rotation == acRhs.Rotation &&
            ChangeFlags == acRhs.ChangeFlags &&
            AppearanceBuffer == acRhs.AppearanceBuffer &&
            GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Cookie{};
    GameId ReferenceId{};
    GameId FormId{};
    GameId CellId{};
    Vector3_NetQuantize Position{};
    Rotator2_NetQuantize Rotation{};
    uint32_t ChangeFlags{};
    String AppearanceBuffer{};
    ActionEvent LatestAction{};
};
