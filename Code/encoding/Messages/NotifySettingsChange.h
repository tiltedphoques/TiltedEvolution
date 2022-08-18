#pragma once

#include "Message.h"
#include <Structs/ServerSettings.h>

struct NotifySettingsChange final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySettingsChange;

    NotifySettingsChange() : 
        ServerMessage(Opcode)
    {
    }

    virtual ~NotifySettingsChange() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifySettingsChange& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               Settings == acRhs.Settings;
    }

    ServerSettings Settings{};
};
