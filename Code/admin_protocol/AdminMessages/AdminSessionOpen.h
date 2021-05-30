#pragma once

#include "Message.h"

struct AdminSessionOpen : ServerAdminMessage
{
    static constexpr ServerAdminOpcode Opcode = kAdminSessionOpen;

    AdminSessionOpen() : ServerAdminMessage(Opcode)
    {
    }

    virtual ~AdminSessionOpen() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AdminSessionOpen& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode();
    }

};
