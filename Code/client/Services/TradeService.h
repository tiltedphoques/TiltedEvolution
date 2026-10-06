#pragma once

#include <Structs/Inventory.h>
#include <Messages/NotifyTradeCancel.h>

#include <optional>

struct World;
struct TransportService;

struct UpdateEvent;
struct DisconnectedEvent;
struct NotifyTradeInvite;
struct NotifyTradeStarted;
struct NotifyTradeState;
struct NotifyTradeCancel;
struct NotifyTradeComplete;

/**
 * @brief Handles client-side trade state and messaging to the UI overlay.
 */
struct TradeService
{
    TradeService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~TradeService() noexcept = default;

    TP_NOCOPYMOVE(TradeService);

    void SendInvite(uint32_t aTargetPlayerId) const noexcept;
    void RespondToInvite(uint32_t aRequesterPlayerId, bool aAccept) const noexcept;
    void CancelTrade() const noexcept;
    void SetReady(bool aReady) const noexcept;
    struct OfferSelection
    {
        uint32_t Index{0};
        int32_t Count{0};
    };

    void UpdateOffer(const TiltedPhoques::Vector<OfferSelection>& aSelections) noexcept;

private:
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnTradeInvite(const NotifyTradeInvite& acMessage) noexcept;
    void OnTradeStarted(const NotifyTradeStarted& acMessage) noexcept;
    void OnTradeState(const NotifyTradeState& acMessage) noexcept;
    void OnTradeCancel(const NotifyTradeCancel& acMessage) noexcept;
    void OnTradeComplete(const NotifyTradeComplete& acMessage) noexcept;

    void ClearSession() noexcept;
    void EmitStateToUI() const noexcept;
    void EmitInviteUpdate(uint32_t aInviterId, bool aAdded, uint64_t aExpiryTick = 0) const noexcept;
    void EmitCancellation(uint32_t aPartnerId, TradeCancelReason aReason, bool aWasInitiator) const noexcept;
    bool HasActiveSessionWith(uint32_t aPartnerId) const noexcept;
    void ApplyOfferSelection(const TiltedPhoques::Vector<OfferSelection>& aSelections);

    World& m_world;
    TransportService& m_transport;

    struct TradeSession
    {
        bool Active{false};
        uint32_t PartnerId{0};
        bool InitiatedBySelf{false};
        bool SelfReady{false};
        bool PartnerReady{false};
        TiltedPhoques::Vector<Inventory::Entry> SelfItems;
        TiltedPhoques::Vector<Inventory::Entry> PartnerItems;
        TiltedPhoques::Vector<Inventory::Entry> SelfInventory;
        uint32_t CountdownMs{0};
        uint32_t CountdownTotalMs{0};
    };

    TradeSession m_session;
    TiltedPhoques::Map<uint32_t, uint64_t> m_pendingInvites;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectConnection;
    entt::scoped_connection m_tradeInviteConnection;
    entt::scoped_connection m_tradeStartedConnection;
    entt::scoped_connection m_tradeStateConnection;
    entt::scoped_connection m_tradeCancelConnection;
    entt::scoped_connection m_tradeCompleteConnection;
};
