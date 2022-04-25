#pragma once

using TiltedPhoques::ConnectionId_t;

struct Player;

/**
* @brief Wrapper for client-to-server messages.
*/
template<class T>
struct PacketEvent
{
    PacketEvent(T* aPacket, Player* apPlayer) : Packet(*aPacket), pPlayer(apPlayer)
    {}

    constexpr PacketEvent(const PacketEvent& acRhs) = default;

    constexpr PacketEvent& operator=(const PacketEvent& acRhs)
    {
        Packet = acRhs.Packet;
        pPlayer = acRhs.pPlayer;

        return *this;
    }

    Player* GetSender() const
    {
        return pPlayer;
    }

    T& Packet;
    Player* pPlayer;
};
