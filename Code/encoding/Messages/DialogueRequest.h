#pragma once

#include "Message.h"
#include <Structs/GameId.h>

using TiltedPhoques::String;

struct DialogueRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kDialogueRequest;

    DialogueRequest()
        : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const DialogueRequest& acRhs) const noexcept { return GetOpcode() == acRhs.GetOpcode() && ServerId == acRhs.ServerId && SoundFilename == acRhs.SoundFilename && SceneId == acRhs.SceneId; }

    uint32_t ServerId{};
    TiltedPhoques::String SoundFilename{};
    // The scene the line was spoken in on the sender, if not in the player dialogue menu
    GameId SceneId{};
};
