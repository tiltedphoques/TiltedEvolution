#include <Services/TradeService.h>
#include <Structs/TradeValidation.h>

#include <Components.h>
#include <World.h>
#include <GameServer.h>

#include <Events/PlayerLeaveEvent.h>
#include <Events/UpdateEvent.h>

#include <Messages/NotifyTradeInvite.h>
#include <Messages/NotifyTradeStarted.h>
#include <Messages/NotifyTradeState.h>
#include <Messages/NotifyTradeComplete.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/TradeInviteRequest.h>
#include <Messages/TradeInviteResponseRequest.h>
#include <Messages/TradeOfferUpdateRequest.h>
#include <Messages/TradeSetReadyRequest.h>
#include <Messages/TradeCancelRequest.h>

#include <Game/Player.h>
#include <Game/PlayerManager.h>

#include <cmath>
#include <cstdlib>
#include <spdlog/spdlog.h>

namespace
{
constexpr uint64_t kTradeInviteDurationMs = 15000;
constexpr uint32_t kTradeFinalizeCountdownMs = 4000;
constexpr uint64_t kCountdownBroadcastIntervalMs = 200;
}

TradeService::TradeService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&TradeService::OnUpdate>(this);
    m_playerLeaveConnection = aDispatcher.sink<PlayerLeaveEvent>().connect<&TradeService::OnPlayerLeave>(this);
    m_tradeInviteConnection = aDispatcher.sink<PacketEvent<TradeInviteRequest>>().connect<&TradeService::OnTradeInvite>(this);
    m_tradeInviteResponseConnection = aDispatcher.sink<PacketEvent<TradeInviteResponseRequest>>().connect<&TradeService::OnTradeInviteResponse>(this);
    m_tradeOfferUpdateConnection = aDispatcher.sink<PacketEvent<TradeOfferUpdateRequest>>().connect<&TradeService::OnTradeOfferUpdate>(this);
    m_tradeSetReadyConnection = aDispatcher.sink<PacketEvent<TradeSetReadyRequest>>().connect<&TradeService::OnTradeSetReady>(this);
    m_tradeCancelConnection = aDispatcher.sink<PacketEvent<TradeCancelRequest>>().connect<&TradeService::OnTradeCancel>(this);
}

void TradeService::OnUpdate(const UpdateEvent&) noexcept
{
    const auto cCurrentTick = GameServer::Get()->GetTick();

    auto it = std::begin(m_pendingInvites);
    while (it != std::end(m_pendingInvites))
    {
        if (it->second.ExpiryTick <= cCurrentTick)
        {
            auto* pTarget = it->first;
            auto* pRequester = it->second.Requester;

            spdlog::debug("[TradeService]: Invite between {} and {} expired", pRequester ? pRequester->GetId() : 0, pTarget ? pTarget->GetId() : 0);

            if (pRequester)
                SendTradeCancelled(pRequester, pTarget, TradeCancelReason::Timeout, true);
            if (pTarget)
                SendTradeCancelled(pTarget, pRequester, TradeCancelReason::Timeout, false);

            it = m_pendingInvites.erase(it);
        }
        else
        {
            ++it;
        }
    }

    TiltedPhoques::Vector<uint32_t> finalizeSessions;
    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
    {
        const uint32_t id = it->first;
        auto* pSession = const_cast<TradeSession*>(&it->second);
        if (!pSession || !pSession->CountdownActive)
            continue;

        const uint32_t remaining = GetRemainingCountdownMs(*pSession, cCurrentTick);
        if (remaining == 0)
        {
            pSession->CountdownActive = false;
            finalizeSessions.push_back(id);
            continue;
        }

        if (cCurrentTick - pSession->LastCountdownBroadcastTick >= kCountdownBroadcastIntervalMs)
        {
            pSession->LastCountdownBroadcastTick = cCurrentTick;
            SendStateUpdate(*pSession);
        }
    }

    for (const auto sessionId : finalizeSessions)
    {
        auto sessionIt = m_sessions.find(sessionId);
        if (sessionIt == std::end(m_sessions))
            continue;

        FinalizeTrade(const_cast<TradeSession&>(sessionIt->second));
    }
}

void TradeService::OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept
{
    auto* pPlayer = acEvent.pPlayer;
    if (!pPlayer)
        return;

    if (auto* pSession = GetSession(pPlayer))
    {
        spdlog::debug("[TradeService]: Player {} left during trade, cancelling session {}", pPlayer->GetId(), pSession->Id);
        CancelSession(*pSession, TradeCancelReason::PlayerLeft, pPlayer);
    }

    RemoveInviteFor(pPlayer, TradeCancelReason::PlayerLeft, pPlayer);
}

void TradeService::OnTradeInvite(const PacketEvent<TradeInviteRequest>& acPacket) noexcept
{
    Player* pRequester = acPacket.pPlayer;
    if (!pRequester)
        return;

    const auto& message = acPacket.Packet;

    Player* pTarget = m_world.GetPlayerManager().GetById(message.TargetPlayerId);
    if (!pTarget || pTarget == pRequester)
    {
        SendTradeCancelled(pRequester, pTarget, TradeCancelReason::FailedValidation, true);
        return;
    }

    if (IsPlayerBusy(pRequester))
    {
        SendTradeCancelled(pRequester, pTarget, TradeCancelReason::SelfBusy, true);
        return;
    }

    if (IsPlayerBusy(pTarget) || m_pendingInvites.find(pTarget) != std::end(m_pendingInvites))
    {
        SendTradeCancelled(pRequester, pTarget, TradeCancelReason::PartnerBusy, true);
        return;
    }

    if (HasOutgoingInvite(pRequester))
    {
        SendTradeCancelled(pRequester, pTarget, TradeCancelReason::SelfBusy, true);
        return;
    }

    const auto cExpiryTick = GameServer::Get()->GetTick() + kTradeInviteDurationMs;
    m_pendingInvites[pTarget] = PendingInvite{pRequester, cExpiryTick};

    spdlog::debug("[TradeService]: Player {} invited {} to trade", pRequester->GetId(), pTarget->GetId());

    NotifyTradeInvite notify;
    notify.InviterPlayerId = pRequester->GetId();
    notify.ExpiryTick = cExpiryTick;
    pTarget->Send(notify);
}

void TradeService::OnTradeInviteResponse(const PacketEvent<TradeInviteResponseRequest>& acPacket) noexcept
{
    Player* pResponder = acPacket.pPlayer;
    if (!pResponder)
        return;

    const auto& message = acPacket.Packet;

    auto it = m_pendingInvites.find(pResponder);
    if (it == std::end(m_pendingInvites))
    {
        SendTradeCancelled(pResponder, nullptr, TradeCancelReason::FailedValidation, false);
        return;
    }

    Player* pRequester = it->second.Requester;
    if (!pRequester || pRequester->GetId() != message.RequesterPlayerId)
    {
        SendTradeCancelled(pResponder, pRequester, TradeCancelReason::FailedValidation, false);
        if (pRequester)
            SendTradeCancelled(pRequester, pResponder, TradeCancelReason::FailedValidation, true);
        m_pendingInvites.erase(it);
        return;
    }

    PendingInvite invite = it->second;
    m_pendingInvites.erase(it);

    if (invite.ExpiryTick <= GameServer::Get()->GetTick())
    {
        SendTradeCancelled(pRequester, pResponder, TradeCancelReason::Timeout, true);
        SendTradeCancelled(pResponder, pRequester, TradeCancelReason::Timeout, false);
        return;
    }

    if (!message.Accept)
    {
        spdlog::debug("[TradeService]: Player {} declined trade invite from {}", pResponder->GetId(), pRequester->GetId());
        SendTradeCancelled(pRequester, pResponder, TradeCancelReason::Declined, true);
        SendTradeCancelled(pResponder, pRequester, TradeCancelReason::Declined, false);
        return;
    }

    if (IsPlayerBusy(pRequester) || IsPlayerBusy(pResponder))
    {
        spdlog::warn("[TradeService]: Trade invite accept failed because one of the players is busy");
        SendTradeCancelled(pRequester, pResponder, TradeCancelReason::PartnerBusy, true);
        SendTradeCancelled(pResponder, pRequester, TradeCancelReason::PartnerBusy, false);
        return;
    }

    auto* pSession = CreateSession(pRequester, pResponder);
    if (!pSession)
    {
        spdlog::error("[TradeService]: Failed to create trade session between {} and {}", pRequester->GetId(), pResponder->GetId());
        SendTradeCancelled(pRequester, pResponder, TradeCancelReason::FailedValidation, true);
        SendTradeCancelled(pResponder, pRequester, TradeCancelReason::FailedValidation, false);
        return;
    }

    spdlog::debug("[TradeService]: Created trade session {} between {} and {}", pSession->Id, pRequester->GetId(), pResponder->GetId());

    SendTradeStarted(*pSession);
    SendStateUpdate(*pSession);
}

void TradeService::OnTradeOfferUpdate(const PacketEvent<TradeOfferUpdateRequest>& acPacket) noexcept
{
    Player* pPlayer = acPacket.pPlayer;
    if (!pPlayer)
        return;

    TradeSession* pSession = GetSession(pPlayer);
    if (!pSession)
        return;

    const int32_t cIndex = GetSessionIndex(*pSession, pPlayer);
    if (cIndex < 0)
        return;

    const auto& message = acPacket.Packet;

    if (!ValidateOffer(pPlayer, message.Items))
    {
        CancelSession(*pSession, TradeCancelReason::FailedValidation, pPlayer);
        return;
    }
    const auto* inventory = GetInventoryFor(pPlayer);
    TiltedPhoques::Vector<Inventory::Entry> sanitized;
    sanitized.reserve(message.Items.size());
    for (const auto& requested : message.Items)
    {
        const auto it = std::find_if(inventory->Entries.begin(), inventory->Entries.end(),
            [&requested](const auto& entry) { return SameTradeItem(entry, requested); });
        auto canonical = *it;
        canonical.Count = requested.Count;
        sanitized.push_back(std::move(canonical));
    }

    pSession->Offers[cIndex].Items = std::move(sanitized);
    pSession->Offers[cIndex].Ready = false;
    pSession->Offers[1 - cIndex].Ready = false;
    ResetCountdown(*pSession);

    spdlog::debug("[TradeService]: Updated offer for player {} in session {}", pPlayer->GetId(), pSession->Id);

    SendStateUpdate(*pSession);
}

void TradeService::OnTradeSetReady(const PacketEvent<TradeSetReadyRequest>& acPacket) noexcept
{
    Player* pPlayer = acPacket.pPlayer;
    if (!pPlayer)
        return;

    TradeSession* pSession = GetSession(pPlayer);
    if (!pSession)
        return;

    const int32_t cIndex = GetSessionIndex(*pSession, pPlayer);
    if (cIndex < 0)
        return;

    const auto& message = acPacket.Packet;

    if (!message.Ready)
    {
        pSession->Offers[cIndex].Ready = false;
        ResetCountdown(*pSession);
        SendStateUpdate(*pSession);
        return;
    }

    if (!ValidateOffer(pPlayer, pSession->Offers[cIndex].Items))
    {
        spdlog::warn("[TradeService]: Player {} attempted to ready invalid offer", pPlayer->GetId());
        CancelSession(*pSession, TradeCancelReason::FailedValidation, pPlayer);
        return;
    }

    if (!ValidatePreparedPotions(pSession->Offers[1 - cIndex].Items, message.PreparedPotions))
    {
        CancelSession(*pSession, TradeCancelReason::FailedValidation, pPlayer);
        return;
    }
    pSession->Offers[cIndex].PreparedPotions = message.PreparedPotions;
    pSession->Offers[cIndex].Ready = true;

    const bool cBothReady = pSession->Offers[0].Ready && pSession->Offers[1].Ready;
    if (cBothReady)
        StartCountdown(*pSession);
    else
        ResetCountdown(*pSession);

    SendStateUpdate(*pSession);
}

void TradeService::OnTradeCancel(const PacketEvent<TradeCancelRequest>& acPacket) noexcept
{
    Player* pPlayer = acPacket.pPlayer;
    if (!pPlayer)
        return;

    if (auto* pSession = GetSession(pPlayer))
    {
        spdlog::debug("[TradeService]: Player {} cancelled trade session {}", pPlayer->GetId(), pSession->Id);
        CancelSession(*pSession, TradeCancelReason::Cancelled, pPlayer);
        return;
    }

    RemoveInviteFor(pPlayer, TradeCancelReason::Cancelled, pPlayer);
}

TradeService::TradeSession* TradeService::GetSession(Player* apPlayer) noexcept
{
    if (!apPlayer)
        return nullptr;

    auto it = m_playerToSession.find(apPlayer);
    if (it == std::end(m_playerToSession))
        return nullptr;

    auto sessionIt = m_sessions.find(it->second);
    if (sessionIt == std::end(m_sessions))
        return nullptr;

    return const_cast<TradeSession*>(&sessionIt->second);
}

const TradeService::TradeSession* TradeService::GetSession(const Player* apPlayer) const noexcept
{
    if (!apPlayer)
        return nullptr;

    auto it = m_playerToSession.find(const_cast<Player*>(apPlayer));
    if (it == std::end(m_playerToSession))
        return nullptr;

    auto sessionIt = m_sessions.find(it->second);
    if (sessionIt == std::end(m_sessions))
        return nullptr;

    return &sessionIt->second;
}

int32_t TradeService::GetSessionIndex(const TradeSession& aSession, const Player* apPlayer) const noexcept
{
    if (apPlayer == aSession.Players[0])
        return 0;
    if (apPlayer == aSession.Players[1])
        return 1;
    return -1;
}

TradeService::TradeSession* TradeService::CreateSession(Player* apInitiator, Player* apPartner) noexcept
{
    if (!apInitiator || !apPartner)
        return nullptr;

    const uint32_t cSessionId = m_nextSessionId++;
    auto result = m_sessions.insert({cSessionId, TradeSession{}});
    if (!result.second)
        return nullptr;

    auto* pSession = const_cast<TradeSession*>(&result.first->second);
    pSession->Id = cSessionId;
    pSession->Players[0] = apInitiator;
    pSession->Players[1] = apPartner;
    pSession->Initiator = apInitiator;
    pSession->CountdownActive = false;
    pSession->CountdownStartTick = 0;
    pSession->CountdownDurationMs = kTradeFinalizeCountdownMs;
    pSession->LastCountdownBroadcastTick = 0;

    m_playerToSession[apInitiator] = cSessionId;
    m_playerToSession[apPartner] = cSessionId;

    return pSession;
}

void TradeService::DestroySession(TradeSession& aSession) noexcept
{
    for (auto* pPlayer : aSession.Players)
    {
        if (pPlayer)
            m_playerToSession.erase(pPlayer);
    }

    m_sessions.erase(aSession.Id);
}

void TradeService::CancelSession(TradeSession& aSession, TradeCancelReason aReason, Player* apOriginator) noexcept
{
    ResetCountdown(aSession);
    TradeSession sessionCopy = aSession;
    DestroySession(aSession);

    for (auto* pPlayer : sessionCopy.Players)
    {
        if (!pPlayer)
            continue;

        Player* pPartner = (pPlayer == sessionCopy.Players[0]) ? sessionCopy.Players[1] : sessionCopy.Players[0];
        const bool cWasInitiator = (pPlayer == sessionCopy.Initiator);
        SendTradeCancelled(pPlayer, pPartner, aReason, cWasInitiator);
    }
}

void TradeService::SendStateUpdate(const TradeSession& aSession) const noexcept
{
    const auto currentTick = GameServer::Get()->GetTick();
    const uint32_t remaining = GetRemainingCountdownMs(aSession, currentTick);
    const uint32_t total = aSession.CountdownActive ? aSession.CountdownDurationMs : 0;

    for (int i = 0; i < 2; ++i)
    {
        Player* pPlayer = aSession.Players[i];
        if (!pPlayer)
            continue;

        Player* pPartner = aSession.Players[1 - i];

        NotifyTradeState notify;
        notify.PartnerPlayerId = pPartner ? pPartner->GetId() : 0;
        notify.SelfReady = aSession.Offers[i].Ready;
        notify.PartnerReady = aSession.Offers[1 - i].Ready;
        notify.SelfItems = aSession.Offers[i].Items;
        notify.PartnerItems = aSession.Offers[1 - i].Items;
        notify.CountdownTotalMs = total;
        notify.CountdownMs = remaining;
        notify.SelfInventory.clear();
        if (auto* pInventory = GetInventoryFor(pPlayer))
            notify.SelfInventory = pInventory->Entries;

        pPlayer->Send(notify);
    }
}

void TradeService::SendTradeStarted(const TradeSession& aSession) const noexcept
{
    for (int i = 0; i < 2; ++i)
    {
        Player* pPlayer = aSession.Players[i];
        if (!pPlayer)
            continue;

        Player* pPartner = aSession.Players[1 - i];

        NotifyTradeStarted notify;
        notify.PartnerPlayerId = pPartner ? pPartner->GetId() : 0;
        notify.InitiatedBySelf = (pPlayer == aSession.Initiator);
        pPlayer->Send(notify);
    }
}

void TradeService::SendTradeCancelled(Player* apRecipient, Player* apPartner, TradeCancelReason aReason, bool aWasInitiator) const noexcept
{
    if (!apRecipient)
        return;

    NotifyTradeCancel notify;
    notify.PartnerPlayerId = apPartner ? apPartner->GetId() : 0;
    notify.Reason = aReason;
    notify.WasInitiator = aWasInitiator;

    apRecipient->Send(notify);
}

void TradeService::FinalizeTrade(TradeSession& aSession) noexcept
{
    TradeSession sessionCopy = aSession;

    auto* leftInventory = GetInventoryFor(sessionCopy.Players[0]);
    auto* rightInventory = GetInventoryFor(sessionCopy.Players[1]);
    Inventory leftResult;
    Inventory rightResult;
    if (!leftInventory || !rightInventory ||
        !ValidatePreparedPotions(sessionCopy.Offers[1].Items, sessionCopy.Offers[0].PreparedPotions) ||
        !ValidatePreparedPotions(sessionCopy.Offers[0].Items, sessionCopy.Offers[1].PreparedPotions) ||
        !PrepareTradeExchange(*leftInventory, *rightInventory,
        sessionCopy.Offers[0].Items, sessionCopy.Offers[1].Items, leftResult, rightResult))
    {
        CancelSession(aSession, TradeCancelReason::FailedValidation);
        return;
    }

    // Both inventories have been prepared and checked before any live state changes.
    *leftInventory = std::move(leftResult);
    *rightInventory = std::move(rightResult);
    for (int from = 0; from < 2; ++from)
    {
        Player* sender = sessionCopy.Players[from];
        Player* recipient = sessionCopy.Players[1 - from];
        const auto senderEntity = *sender->GetCharacter();
        const auto recipientEntity = *recipient->GetCharacter();
        for (const auto& item : sessionCopy.Offers[from].Items)
        {
            NotifyInventoryChanges removal;
            removal.ServerId = World::ToInteger(senderEntity);
            removal.OwnershipEpoch = m_world.get<OwnerComponent>(senderEntity).OwnershipEpoch;
            removal.Item = item;
            removal.Item.Count = -item.Count;
            sender->Send(removal);
            if (!GameServer::Get()->SendToPlayersInRange(removal, senderEntity, sender))
                spdlog::error("[TradeService]: Failed to broadcast removal for {}", sender->GetId());

            NotifyInventoryChanges addition;
            addition.ServerId = World::ToInteger(recipientEntity);
            addition.OwnershipEpoch = m_world.get<OwnerComponent>(recipientEntity).OwnershipEpoch;
            addition.Item = item;
            recipient->Send(addition);
            if (!GameServer::Get()->SendToPlayersInRange(addition, recipientEntity, recipient))
                spdlog::error("[TradeService]: Failed to broadcast addition for {}", recipient->GetId());
        }
    }

    DestroySession(aSession);

    for (int i = 0; i < 2; ++i)
    {
        Player* pPlayer = sessionCopy.Players[i];
        if (!pPlayer)
            continue;

        Player* pPartner = sessionCopy.Players[1 - i];

        NotifyTradeComplete notify;
        notify.PartnerPlayerId = pPartner ? pPartner->GetId() : 0;
        pPlayer->Send(notify);
    }
}

bool TradeService::ValidateOffer(Player* apPlayer, const TiltedPhoques::Vector<Inventory::Entry>& aItems) const noexcept
{
    auto* pInventory = GetInventoryFor(apPlayer);
    return pInventory && ValidateTradeOffer(*pInventory, aItems);
}

Inventory* TradeService::GetInventoryFor(Player* apPlayer) const noexcept
{
    if (!apPlayer)
        return nullptr;

    auto character = apPlayer->GetCharacter();
    if (!character)
        return nullptr;

    if (!m_world.valid(*character) || !m_world.all_of<InventoryComponent, OwnerComponent>(*character))
        return nullptr;

    if (m_world.get<OwnerComponent>(*character).GetOwner() != apPlayer)
        return nullptr;

    auto& component = m_world.get<InventoryComponent>(*character);
    return const_cast<Inventory*>(&component.Content);
}

bool TradeService::IsPlayerBusy(Player* apPlayer) const noexcept
{
    if (!apPlayer)
        return false;

    if (m_playerToSession.find(const_cast<Player*>(apPlayer)) != std::end(m_playerToSession))
        return true;

    if (m_pendingInvites.find(const_cast<Player*>(apPlayer)) != std::end(m_pendingInvites))
        return true;

    if (HasOutgoingInvite(apPlayer))
        return true;

    return false;
}

bool TradeService::HasOutgoingInvite(Player* apPlayer) const noexcept
{
    if (!apPlayer)
        return false;

    for (const auto& [target, invite] : m_pendingInvites)
    {
        if (invite.Requester == apPlayer)
            return true;
    }

    return false;
}

void TradeService::RemoveInviteFor(Player* apPlayer, TradeCancelReason aReason, Player* apOriginator) noexcept
{
    if (!apPlayer)
        return;

    auto targetIt = m_pendingInvites.find(apPlayer);
    if (targetIt != std::end(m_pendingInvites))
    {
        Player* pRequester = targetIt->second.Requester;
        m_pendingInvites.erase(targetIt);

        if (pRequester)
            SendTradeCancelled(pRequester, apPlayer, aReason, true);
        SendTradeCancelled(apPlayer, pRequester, aReason, false);
        return;
    }

    auto it = std::begin(m_pendingInvites);
    while (it != std::end(m_pendingInvites))
    {
        if (it->second.Requester == apPlayer)
        {
            Player* pTarget = it->first;
            SendTradeCancelled(pTarget, apPlayer, aReason, false);
            SendTradeCancelled(apPlayer, pTarget, aReason, true);
            it = m_pendingInvites.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void TradeService::StartCountdown(TradeSession& aSession) noexcept
{
    const uint64_t currentTick = GameServer::Get()->GetTick();
    aSession.CountdownActive = true;
    aSession.CountdownStartTick = currentTick;
    if (aSession.CountdownDurationMs == 0)
        aSession.CountdownDurationMs = kTradeFinalizeCountdownMs;
    aSession.LastCountdownBroadcastTick = currentTick;
}

void TradeService::ResetCountdown(TradeSession& aSession) noexcept
{
    aSession.CountdownActive = false;
    aSession.CountdownStartTick = 0;
    aSession.LastCountdownBroadcastTick = 0;
}

uint32_t TradeService::GetRemainingCountdownMs(const TradeSession& aSession, uint64_t aCurrentTick) const noexcept
{
    if (!aSession.CountdownActive || aSession.CountdownDurationMs == 0)
        return 0;

    const uint64_t endTick = aSession.CountdownStartTick + aSession.CountdownDurationMs;
    if (aCurrentTick >= endTick)
        return 0;

    return static_cast<uint32_t>(endTick - aCurrentTick);
}
