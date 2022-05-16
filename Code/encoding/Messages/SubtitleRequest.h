#pragma once

#include "Message.h"

using TiltedPhoques::String;

struct SubtitleRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kSubtitleRequest;

    SubtitleRequest() : ClientMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const SubtitleRequest& acRhs) const noexcept
    {
        return GetOpcode() == acRhs.GetOpcode() &&
               ServerId == acRhs.ServerId &&
               Text == acRhs.Text;
    }

    uint32_t ServerId{};
    TiltedPhoques::String Text{};
};

