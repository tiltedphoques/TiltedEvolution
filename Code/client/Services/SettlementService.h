#pragma once

#if defined(TP_FALLOUT4)
#include <Games/Events.h>
#include <Structs/SettlementObject.h>
#include <unordered_map>
#include <optional>

struct World;
struct TransportService;
struct ConnectedEvent;
struct DisconnectedEvent;
struct UpdateEvent;
struct CellChangeEvent;
struct PartyJoinedEvent;
struct PartyLeftEvent;
struct NotifySettlement;

namespace Workshop
{
struct ItemPlacedEvent { TESObjectREFR* Workshop; TESObjectREFR* Item; };
struct ItemMovedEvent { TESObjectREFR* Workshop; TESObjectREFR* Item; };
struct ItemDestroyedEvent { TESObjectREFR* Workshop; TESObjectREFR* Item; };
}

class SettlementService final : public BSTEventSink<Workshop::ItemPlacedEvent>,
                                public BSTEventSink<Workshop::ItemMovedEvent>,
                                public BSTEventSink<Workshop::ItemDestroyedEvent>
{
public:
    SettlementService(World&, entt::dispatcher&, TransportService&);
    ~SettlementService();

private:
    BSTEventResult OnEvent(const Workshop::ItemPlacedEvent*, const EventDispatcher<Workshop::ItemPlacedEvent>*) override;
    BSTEventResult OnEvent(const Workshop::ItemMovedEvent*, const EventDispatcher<Workshop::ItemMovedEvent>*) override;
    BSTEventResult OnEvent(const Workshop::ItemDestroyedEvent*, const EventDispatcher<Workshop::ItemDestroyedEvent>*) override;
    void Capture(TESObjectREFR*, TESObjectREFR*, bool) noexcept;
    void OnConnected(const ConnectedEvent&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnPartyJoined(const PartyJoinedEvent&) noexcept;
    void OnPartyLeft(const PartyLeftEvent&) noexcept;
    void Reset() noexcept;
    void OnCellChange(const CellChangeEvent&) noexcept;
    void OnUpdate(const UpdateEvent&) noexcept;
    void OnNotify(const NotifySettlement&) noexcept;

    struct Record
    {
        SettlementObject Data{};
        uint32_t LocalId{};
        bool Spawned{};
        bool NeedsApply{};
    };
    struct Pending
    {
        SettlementObject Data{};
        bool Waiting{};
    };
    bool Apply(Record&) noexcept;
    void ClearReplicas() noexcept;
    bool ReadObject(TESObjectREFR*, TESObjectREFR*, bool, SettlementObject&) noexcept;
    void ImportLoaded() noexcept;

    World& m_world;
    TransportService& m_transport;
    uint64_t m_sessionId{};
    double m_elapsed{};
    double m_scanElapsed{};
    std::optional<uint32_t> m_leaderId;
    bool m_isLeader{};
    bool m_applying{};
    std::unordered_map<uint32_t, Record> m_objects;
    std::unordered_map<uint32_t, Pending> m_pending;
    entt::scoped_connection m_partyJoinedConnection;
    entt::scoped_connection m_partyLeftConnection;
    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_notifyConnection;
    entt::scoped_connection m_cellChangeConnection;
};
#endif
