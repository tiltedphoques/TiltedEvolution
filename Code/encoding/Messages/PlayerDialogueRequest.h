#pragma once

#include "Message.h"

using TiltedPhoques::String;

struct PlayerDialogueRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kPlayerDialogueRequest;

    PlayerDialogueRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const PlayerDialogueRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               Text == acRhs.Text;
    }

    TiltedPhoques::String Text{};
};

