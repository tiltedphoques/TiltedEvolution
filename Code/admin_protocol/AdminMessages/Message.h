#pragma once

#include "../Opcodes.h"
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::String;
using TiltedPhoques::Serialization;

struct ClientAdminMessage : TiltedPhoques::AllocatorCompatible
{
    ClientAdminMessage(ClientAdminOpcode aOpcode)
        : m_opcode(aOpcode)
    {}

    virtual ~ClientAdminMessage() = default;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    // Serialize values that are not dependent on previous states
    virtual void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    // Serialize values that are dependent on previous states
    virtual void SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    // Deserialize values that are dependent on previous states, this function will already be called 
    virtual void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    virtual void DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] ClientAdminOpcode GetOpcode() const noexcept;

private:
    ClientAdminOpcode m_opcode;
};

struct ServerAdminMessage : TiltedPhoques::AllocatorCompatible
{
    ServerAdminMessage(ServerAdminOpcode aOpcode)
        : m_opcode(aOpcode)
    {}

    virtual ~ServerAdminMessage() = default;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    virtual void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    virtual void SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    virtual void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    virtual void DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] ServerAdminOpcode GetOpcode() const noexcept;

private:
    ServerAdminOpcode m_opcode;
};
