#pragma once

#include "../Opcodes.h"
#include "../ChatMessageTypes.h"

using TiltedPhoques::String;
using TiltedPhoques::Serialization;

struct ClientMessage : TiltedPhoques::AllocatorCompatible
{
    ClientMessage(ClientOpcode aOpcode)
        : m_opcode(aOpcode)
    {}

    virtual ~ClientMessage() = default;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    // Serialize values that are not dependent on previous states
    virtual void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    // Serialize values that are dependent on previous states
    virtual void SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    // Deserialize values that are dependent on previous states, this function will already be called 
    virtual void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    virtual void DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] ClientOpcode GetOpcode() const noexcept;

private:
    ClientOpcode m_opcode;
};

struct ServerMessage : TiltedPhoques::AllocatorCompatible
{
    ServerMessage(ServerOpcode aOpcode)
        : m_opcode(aOpcode)
    {}

    virtual ~ServerMessage() = default;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    virtual void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    virtual void SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    virtual void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    virtual void DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] ServerOpcode GetOpcode() const noexcept;

private:
    ServerOpcode m_opcode;
};
