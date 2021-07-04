#pragma once

#include "Message.h"

struct ServerLogs : ServerAdminMessage
{
    static constexpr ServerAdminOpcode Opcode = kServerLogs;

    ServerLogs() : ServerAdminMessage(Opcode)
    {
    }

    virtual ~ServerLogs() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ServerLogs& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() && Logs == achRhs.Logs;
    }

    String Logs;

};
