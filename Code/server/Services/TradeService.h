#pragma once

#include <Events/PacketEvent.h>

#include <Structs/Inventory.h>
#include <Messages/NotifyTradeCancel.h>

struct World;
struct UpdateEvent;
struct PlayerLeaveEvent;
struct TradeInviteRequest;
struct TradeInviteResponseRequest;
struct TradeOfferUpdateRequest;
struct TradeSetReadyRequest;
struct TradeCancelRequest;

struct Player;

/**
 * @brief Handles player to player trading sessions.
 */
class TradeService
{
public:
    TradeService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~TradeService() noexcept = default;

    TP_NOCOPYMOVE(TradeService);

protected:
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept;
    void OnTradeInvite(const PacketEvent<TradeInviteRequest>& acPacket) noexcept;
    void OnTradeInviteResponse(const PacketEvent<TradeInviteResponseRequest>& acPacket) noexcept;
    void OnTradeOfferUpdate(const PacketEvent<TradeOfferUpdateRequest>& acPacket) noexcept;
    void OnTradeSetReady(const PacketEvent<TradeSetReadyRequest>& acPacket) noexcept;
    void OnTradeCancel(const PacketEvent<TradeCancelRequest>& acPacket) noexcept;

private:
    struct TradeOffer
    {
        // GCC evaluates the map's value traits before the enclosing service is complete.
        TradeOffer() noexcept
            : Ready(false)
        {
        }

        TiltedPhoques::Vector<Inventory::Entry> Items;
        bool Ready;
    };

    struct TradeSession
    {
        uint32_t Id{0};
        Player* Players[2]{nullptr, nullptr};
        Player* Initiator{nullptr};
        TradeOffer Offers[2];
        bool CountdownActive{false};
        uint64_t CountdownStartTick{0};
        uint32_t CountdownDurationMs{0};
        uint64_t LastCountdownBroadcastTick{0};
    };

    struct PendingInvite
    {
        Player* Requester{nullptr};
        uint64_t ExpiryTick{0};
    };

    TradeSession* GetSession(Player* apPlayer) noexcept;
    const TradeSession* GetSession(const Player* apPlayer) const noexcept;
    int32_t GetSessionIndex(const TradeSession& aSession, const Player* apPlayer) const noexcept;
    TradeSession* CreateSession(Player* apInitiator, Player* apPartner) noexcept;
    void DestroySession(TradeSession& aSession) noexcept;
    void CancelSession(TradeSession& aSession, TradeCancelReason aReason, Player* apOriginator = nullptr) noexcept;
    void SendStateUpdate(const TradeSession& aSession) const noexcept;
    void SendTradeStarted(const TradeSession& aSession) const noexcept;
    void SendTradeCancelled(Player* apRecipient, Player* apPartner, TradeCancelReason aReason, bool aWasInitiator) const noexcept;
    void FinalizeTrade(TradeSession& aSession) noexcept;
    bool ValidateOffer(Player* apPlayer, const TiltedPhoques::Vector<Inventory::Entry>& aItems) const noexcept;
    Inventory* GetInventoryFor(Player* apPlayer) const noexcept;
    bool IsPlayerBusy(Player* apPlayer) const noexcept;
    bool HasOutgoingInvite(Player* apPlayer) const noexcept;
    void RemoveInviteFor(Player* apPlayer, TradeCancelReason aReason, Player* apOriginator = nullptr) noexcept;
    void StartCountdown(TradeSession& aSession) noexcept;
    void ResetCountdown(TradeSession& aSession) noexcept;
    uint32_t GetRemainingCountdownMs(const TradeSession& aSession, uint64_t aCurrentTick) const noexcept;

    World& m_world;

    TiltedPhoques::Map<uint32_t, TradeSession> m_sessions;
    TiltedPhoques::Map<Player*, uint32_t> m_playerToSession;
    TiltedPhoques::Map<Player*, PendingInvite> m_pendingInvites;
    uint32_t m_nextSessionId{1};

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_playerLeaveConnection;
    entt::scoped_connection m_tradeInviteConnection;
    entt::scoped_connection m_tradeInviteResponseConnection;
    entt::scoped_connection m_tradeOfferUpdateConnection;
    entt::scoped_connection m_tradeSetReadyConnection;
    entt::scoped_connection m_tradeCancelConnection;
};
