#pragma once


using TiltedPhoques::ConnectionId_t;

template<class T>
struct PacketEvent
{
    PacketEvent(T* aPacket, ConnectionId_t aConnectionId)
        : Packet(*aPacket)
        , ConnectionId(aConnectionId)
    {}

    constexpr PacketEvent(const PacketEvent& acRhs) = default;

    constexpr PacketEvent& operator=(const PacketEvent& acRhs)
    {
        Packet = acRhs.Packet;
        ConnectionId = acRhs.ConnectionId;

        return *this;
    }

    T& Packet;
    ConnectionId_t ConnectionId;
};
