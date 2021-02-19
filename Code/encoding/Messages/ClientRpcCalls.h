#pragma once

#include "Message.h"

struct ClientRpcCalls final : ClientMessage
{
    ClientRpcCalls()
        : ClientMessage(kClientRpcCalls)
    {
    }

    virtual ~ClientRpcCalls() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const ClientRpcCalls& achRhs) const noexcept
    {
        return Data == achRhs.Data &&
            GetOpcode() == achRhs.GetOpcode();
    }

    String Data{};
};
