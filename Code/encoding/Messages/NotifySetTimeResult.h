#pragma once

#include "Message.h"

struct NotifySetTimeResult final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifySetTimeResult;

    NotifySetTimeResult() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifySetTimeResult& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && Result == achRhs.Result;
    }

    enum class SetTimeResult : uint8_t
    {
        kSuccess,
        kNoPermission
    };

    SetTimeResult Result{};
};
