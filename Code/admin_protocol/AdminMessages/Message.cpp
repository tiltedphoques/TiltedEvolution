#include "Message.h"

void ClientAdminMessage::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    // We don't read the opcode, the factory will do it
}

void ClientAdminMessage::DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
}

ClientAdminOpcode ClientAdminMessage::GetOpcode() const noexcept
{
    return m_opcode;
}

void ClientAdminMessage::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    ClientAdminMessage::SerializeRaw(aWriter);
    SerializeRaw(aWriter);
    SerializeDifferential(aWriter);
}

void ClientAdminMessage::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(m_opcode, sizeof(m_opcode) * 8);
}

void ClientAdminMessage::SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void ServerAdminMessage::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    // We don't read the opcode, the factory will do it
}

void ServerAdminMessage::DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
}

ServerAdminOpcode ServerAdminMessage::GetOpcode() const noexcept
{
    return m_opcode;
}

void ServerAdminMessage::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    ServerAdminMessage::SerializeRaw(aWriter);
    SerializeRaw(aWriter);
    SerializeDifferential(aWriter);
}

void ServerAdminMessage::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(m_opcode, sizeof(m_opcode) * 8);
}

void ServerAdminMessage::SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}



