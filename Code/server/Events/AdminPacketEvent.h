#pragma once

using TiltedPhoques::ConnectionId_t;

/**
* @brief Wrapper for admin messages.
*/
template<class T>
struct AdminPacketEvent
{
    AdminPacketEvent(T* aPacket, ConnectionId_t aConnectionId)
        : Packet(*aPacket), ConnectionId(aConnectionId)
    {}

    constexpr AdminPacketEvent(const AdminPacketEvent& acRhs) = default;

    constexpr AdminPacketEvent& operator=(const AdminPacketEvent& acRhs)
    {
        Packet = acRhs.Packet;
        ConnectionId = acRhs.ConnectionId;

        return *this;
    }

    T& Packet;
    ConnectionId_t ConnectionId;
};
