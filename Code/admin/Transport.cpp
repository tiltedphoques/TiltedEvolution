#include "AdminApp.h"
#include "Packet.hpp"
#include "AdminMessages/AdminShutdownRequest.h"
#include "AdminMessages/ServerLogs.h"
#include "AdminMessages/ServerAdminMessageFactory.h"

#include <Messages/AuthenticationRequest.h>

#include <TiltedCore/ScratchAllocator.hpp>
#include <TiltedCore/ViewBuffer.hpp>

void AdminApp::OnConsume(const void* apData, uint32_t aSize)
{
    ServerAdminMessageFactory factory;
    TiltedPhoques::ViewBuffer buf((uint8_t*)apData, aSize);
    TiltedPhoques::Buffer::Reader reader(&buf);

    auto pMessage = factory.Extract(reader);
    if (!pMessage)
    {
        return;
    }

    m_messageHandlers[pMessage->GetOpcode()](pMessage);
}

void AdminApp::OnConnected()
{
    AuthenticationRequest request;
    request.Token = String(m_password.c_str());
    m_password = String();
    m_password.resize(1024);

    Send(request);
}

void AdminApp::OnDisconnected(EDisconnectReason aReason)
{
    m_state = ConnectionState::kNone;
}

void AdminApp::OnUpdate()
{
}

bool AdminApp::Send(const ClientAdminMessage& acMessage) const noexcept
{
    static thread_local TiltedPhoques::ScratchAllocator s_allocator(1 << 18);

    struct ScopedReset
    {
        ~ScopedReset()
        {
            s_allocator.Reset();
        }
    } allocatorGuard;

    if (IsConnected())
    {
        TiltedPhoques::ScopedAllocator _{s_allocator};

        TiltedPhoques::Buffer buffer(1 << 16);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        writer.WriteBits(0, 8); // Write first byte as packet needs it

        acMessage.Serialize(writer);
        TiltedPhoques::PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), writer.Size());

        Client::Send(&packet);

        return true;
    }

    return false;
}

bool AdminApp::Send(const ClientMessage& acMessage) const noexcept
{
    static thread_local TiltedPhoques::ScratchAllocator s_allocator(1 << 18);

    struct ScopedReset
    {
        ~ScopedReset()
        {
            s_allocator.Reset();
        }
    } allocatorGuard;

    if (IsConnected())
    {
        TiltedPhoques::ScopedAllocator _{s_allocator};

        TiltedPhoques::Buffer buffer(1 << 16);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        writer.WriteBits(0, 8); // Write first byte as packet needs it

        acMessage.Serialize(writer);
        TiltedPhoques::PacketView packet(reinterpret_cast<char*>(buffer.GetWriteData()), writer.Size());

        Client::Send(&packet);

        return true;
    }

    return false;
}


void AdminApp::SendShutdownRequest()
{
    AdminShutdownRequest request;
    Send(request);
}

void AdminApp::HandleMessage(const AdminSessionOpen& acMessage)
{
    m_state = ConnectionState::kConnected;
}

void AdminApp::HandleMessage(const ServerLogs& acMessage)
{
    m_overlay.GetConsole().Log(acMessage.Logs);
}
