#pragma once

#include <Events/PacketEvent.h>
#include <Structs/SettlementObject.h>
#include <unordered_map>

struct World;
struct SettlementRequest;
struct UpdateEvent;

class SettlementService
{
public:
    SettlementService(World& aWorld, entt::dispatcher& aDispatcher);

private:
    void OnRequest(const PacketEvent<SettlementRequest>& acEvent) noexcept;
    void OnUpdate(const UpdateEvent&) noexcept;

    struct Record
    {
        SettlementObject Data{};
        uint64_t Origin{};
    };
    struct Session
    {
        uint64_t Id{};
        uint32_t LeaderPlayerId{};
        uint32_t NextId{1};
        std::unordered_map<uint32_t, Record> Objects;
    };

    World& m_world;
    uint64_t m_nextSessionId;
    std::unordered_map<uint32_t, Session> m_sessions;
    entt::scoped_connection m_requestConnection;
    entt::scoped_connection m_updateConnection;
};
