#include <Messages/Message.h>

void ClientMessage::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    // We don't read the opcode, the factory will do it
}

void ClientMessage::DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
}

ClientOpcode ClientMessage::GetOpcode() const noexcept
{
    return m_opcode;
}

void ClientMessage::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    ClientMessage::SerializeRaw(aWriter);
    SerializeRaw(aWriter);
    SerializeDifferential(aWriter);
}

void ClientMessage::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(m_opcode, sizeof(m_opcode) * 8);
}

void ClientMessage::SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void ServerMessage::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    // We don't read the opcode, the factory will do it
}

void ServerMessage::DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
}

ServerOpcode ServerMessage::GetOpcode() const noexcept
{
    return m_opcode;
}

void ServerMessage::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    ServerMessage::SerializeRaw(aWriter);
    SerializeRaw(aWriter);
    SerializeDifferential(aWriter);
}

void ServerMessage::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(m_opcode, sizeof(m_opcode) * 8);
}

void ServerMessage::SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}



