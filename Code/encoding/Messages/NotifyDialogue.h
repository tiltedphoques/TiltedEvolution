#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct NotifyDialogue final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyDialogue;

    NotifyDialogue()
        : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyDialogue& achRhs) const noexcept { return GetOpcode() == achRhs.GetOpcode() && ServerId == achRhs.ServerId && SoundFilename == achRhs.SoundFilename && SceneId == achRhs.SceneId && ScenePhase == achRhs.ScenePhase; }

    uint32_t ServerId{};
    TiltedPhoques::String SoundFilename{};
    // The scene the line was spoken in on the sender, if not in the player dialogue menu
    GameId SceneId{};
    // The sender's current phase of that scene when the line started
    uint32_t ScenePhase{};
};
