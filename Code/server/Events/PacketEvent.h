#pragma once


using TiltedPhoques::ConnectionId_t;

struct PlayerComponent;

template<class T>
struct PacketEvent
{
    PacketEvent(T* aPacket, PlayerComponent& aPlayerComponent, entt::entity aEntity)
        : Packet(*aPacket), PlayerComponent(aPlayerComponent), Entity(aEntity)
    {}

    constexpr PacketEvent(const PacketEvent& acRhs) = default;

    constexpr PacketEvent& operator=(const PacketEvent& acRhs)
    {
        Packet = acRhs.Packet;
        PlayerComponent = acRhs.PlayerComponent;
        Entity = acRhs.Entity;

        return *this;
    }

    T& Packet;
    PlayerComponent& PlayerComponent;
    entt::entity Entity;
};
