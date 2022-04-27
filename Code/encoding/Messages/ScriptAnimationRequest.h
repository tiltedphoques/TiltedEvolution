#pragma once

#include "Message.h"
#include "Structs/CachedString.h"

struct ScriptAnimationRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kScriptAnimationRequest;

    ScriptAnimationRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ScriptAnimationRequest& acRhs) const noexcept
    {
        return FormID == acRhs.FormID &&
               Animation == acRhs.Animation &&
               EventName == acRhs.EventName &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t FormID;
    CachedString Animation;
    CachedString EventName;
};
