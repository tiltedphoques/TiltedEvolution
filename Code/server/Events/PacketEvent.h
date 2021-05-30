#pragma once


using TiltedPhoques::ConnectionId_t;

struct PlayerComponent;

template<class T>
struct PacketEvent
{
    PacketEvent(T* aPacket, PlayerComponent& aPlayerComponent, entt::entity aEntity)
        : Packet(*aPacket), Player(aPlayerComponent), Entity(aEntity)
    {}

    constexpr PacketEvent(const PacketEvent& acRhs) = default;

    constexpr PacketEvent& operator=(const PacketEvent& acRhs)
    {
        Packet = acRhs.Packet;
        Player = acRhs.Player;
        Entity = acRhs.Entity;

        return *this;
    }

    T& Packet;
    PlayerComponent& Player;
    entt::entity Entity;
};
