#pragma once

#include "Message.h"

struct AdminShutdownRequest : ClientAdminMessage
{
    static constexpr ClientAdminOpcode Opcode = kAdminShutdown;

    AdminShutdownRequest() : ClientAdminMessage(Opcode)
    {
    }

    virtual ~AdminShutdownRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AdminShutdownRequest& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode();
    }

};
