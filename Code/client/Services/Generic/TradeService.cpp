#include <TiltedOnlinePCH.h>

#include <Services/TradeService.h>
#include <Structs/TradeValidation.h>

#include <Services/OverlayService.h>
#include <Services/TransportService.h>

#include <Messages/TradeInviteRequest.h>
#include <Messages/TradeInviteResponseRequest.h>
#include <Messages/TradeOfferUpdateRequest.h>
#include <Messages/TradeSetReadyRequest.h>
#include <Messages/TradeCancelRequest.h>
#include <Messages/NotifyTradeInvite.h>
#include <Messages/NotifyTradeStarted.h>
#include <Messages/NotifyTradeState.h>
#include <Messages/NotifyTradeCancel.h>
#include <Messages/NotifyTradeComplete.h>

#include <OverlayApp.hpp>
#include <World.h>

#include <include/cef_values.h>

#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>

#include <Systems/ModSystem.h>
#include <Games/Skyrim/Forms/TESForm.h>

#include <spdlog/fmt/fmt.h>

#include <cmath>
#include <cstring>
#include <string>
#include <optional>
#include <vector>

namespace
{
constexpr uint64_t kInviteCleanupIntervalMs = 1000;

std::string MakeDisplayName(ModSystem& aModSystem, const Inventory::Entry& aEntry)
{
    const uint32_t formId = aModSystem.GetGameId(aEntry.BaseId);
    if (formId)
    {
        if (auto* pForm = TESForm::GetById(formId))
        {
            if (const char* pName = pForm->GetName(); pName && std::strlen(pName) > 0)
                return pName;
        }
    }

    return fmt::format("0x{:08X}:0x{:08X}", aEntry.BaseId.ModId, aEntry.BaseId.BaseId);
}
} // namespace

TradeService::TradeService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&TradeService::OnUpdate>(this);
    m_disconnectConnection = aDispatcher.sink<DisconnectedEvent>().connect<&TradeService::OnDisconnected>(this);
    m_tradeInviteConnection = aDispatcher.sink<NotifyTradeInvite>().connect<&TradeService::OnTradeInvite>(this);
    m_tradeStartedConnection = aDispatcher.sink<NotifyTradeStarted>().connect<&TradeService::OnTradeStarted>(this);
    m_tradeStateConnection = aDispatcher.sink<NotifyTradeState>().connect<&TradeService::OnTradeState>(this);
    m_tradeCancelConnection = aDispatcher.sink<NotifyTradeCancel>().connect<&TradeService::OnTradeCancel>(this);
    m_tradeCompleteConnection = aDispatcher.sink<NotifyTradeComplete>().connect<&TradeService::OnTradeComplete>(this);
}

void TradeService::SendInvite(uint32_t aTargetPlayerId) const noexcept
{
    TradeInviteRequest request{};
    request.TargetPlayerId = aTargetPlayerId;
    m_transport.Send(request);
}

void TradeService::RespondToInvite(uint32_t aRequesterPlayerId, bool aAccept) const noexcept
{
    TradeInviteResponseRequest response{};
    response.RequesterPlayerId = aRequesterPlayerId;
    response.Accept = aAccept;
    m_transport.Send(response);
}

void TradeService::CancelTrade() const noexcept
{
    TradeCancelRequest request{};
    m_transport.Send(request);
}

void TradeService::SetReady(bool aReady) const noexcept
{
    TradeSetReadyRequest request{};
    request.Ready = aReady;
    m_transport.Send(request);
}

void TradeService::UpdateOffer(const TiltedPhoques::Vector<OfferSelection>& aSelections) noexcept
{
    ApplyOfferSelection(aSelections);
}

void TradeService::ApplyOfferSelection(const TiltedPhoques::Vector<OfferSelection>& aSelections)
{
    if (!m_session.Active)
        return;

    TiltedPhoques::Vector<Inventory::Entry> entries;
    entries.reserve(aSelections.size());

    for (const auto& selection : aSelections)
    {
        if (selection.Index >= m_session.SelfInventory.size())
            continue;

        if (selection.Count <= 0)
            continue;

        const auto& sourceEntry = m_session.SelfInventory[selection.Index];
        if (selection.Count > sourceEntry.Count)
            continue;

        Inventory::Entry entry = sourceEntry;
        entry.Count = selection.Count;
        entries.push_back(entry);
    }

    TradeOfferUpdateRequest request{};
    request.Items = entries;
    m_transport.Send(request);

    m_session.SelfItems = request.Items;
    m_session.SelfReady = false;

    EmitStateToUI();
}

void TradeService::OnUpdate(const UpdateEvent&) noexcept
{
    const auto cCurrentTick = m_transport.GetClock().GetCurrentTick();

    static uint64_t s_nextCleanupTick = 0;
    if (s_nextCleanupTick > cCurrentTick)
        return;

    s_nextCleanupTick = cCurrentTick + kInviteCleanupIntervalMs;

    auto it = std::begin(m_pendingInvites);
    while (it != std::end(m_pendingInvites))
    {
        if (it->second <= cCurrentTick)
        {
            const uint32_t inviterId = it->first;
            it = m_pendingInvites.erase(it);
            EmitInviteUpdate(inviterId, false);
        }
        else
        {
            ++it;
        }
    }
}

void TradeService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    if (!m_pendingInvites.empty())
    {
        auto invites = m_pendingInvites;
        m_pendingInvites.clear();
        for (const auto& [inviterId, _] : invites)
            EmitInviteUpdate(inviterId, false);
    }

    if (m_session.Active)
    {
        EmitCancellation(m_session.PartnerId, TradeCancelReason::Cancelled, m_session.InitiatedBySelf);
        ClearSession();
    }
}

void TradeService::OnTradeInvite(const NotifyTradeInvite& acMessage) noexcept
{
    m_pendingInvites[acMessage.InviterPlayerId] = acMessage.ExpiryTick;
    EmitInviteUpdate(acMessage.InviterPlayerId, true, acMessage.ExpiryTick);
}

void TradeService::OnTradeStarted(const NotifyTradeStarted& acMessage) noexcept
{
    m_session.Active = true;
    m_session.PartnerId = acMessage.PartnerPlayerId;
    m_session.InitiatedBySelf = acMessage.InitiatedBySelf;
    m_session.SelfReady = false;
    m_session.PartnerReady = false;
    m_session.SelfItems.clear();
    m_session.PartnerItems.clear();
    m_session.SelfInventory.clear();
    m_session.CountdownMs = 0;
    m_session.CountdownTotalMs = 0;

    m_pendingInvites.erase(acMessage.PartnerPlayerId);
    EmitInviteUpdate(acMessage.PartnerPlayerId, false);

    EmitStateToUI();
}

void TradeService::OnTradeState(const NotifyTradeState& acMessage) noexcept
{
    if (!m_session.Active || m_session.PartnerId != acMessage.PartnerPlayerId)
    {
        m_session.Active = true;
        m_session.PartnerId = acMessage.PartnerPlayerId;
    }

    m_session.SelfReady = acMessage.SelfReady;
    m_session.PartnerReady = acMessage.PartnerReady;
    m_session.SelfItems = acMessage.SelfItems;
    m_session.PartnerItems = acMessage.PartnerItems;
    m_session.SelfInventory = acMessage.SelfInventory;
    m_session.CountdownMs = acMessage.CountdownMs;
    m_session.CountdownTotalMs = acMessage.CountdownTotalMs;

    EmitStateToUI();
}

void TradeService::OnTradeCancel(const NotifyTradeCancel& acMessage) noexcept
{
    EmitCancellation(acMessage.PartnerPlayerId, acMessage.Reason, acMessage.WasInitiator);

    if (HasActiveSessionWith(acMessage.PartnerPlayerId))
        ClearSession();
}

void TradeService::OnTradeComplete(const NotifyTradeComplete& acMessage) noexcept
{
    auto* pOverlay = m_world.GetOverlayService().GetOverlayApp();
    if (pOverlay)
    {
        auto pArgs = CefListValue::Create();
        pArgs->SetInt(0, acMessage.PartnerPlayerId);
        pOverlay->ExecuteAsync("tradeCompleted", pArgs);
    }

    if (HasActiveSessionWith(acMessage.PartnerPlayerId))
        ClearSession();
}

void TradeService::ClearSession() noexcept
{
    m_session = TradeSession{};
    EmitStateToUI();
}

void TradeService::EmitStateToUI() const noexcept
{
    auto* pOverlay = m_world.GetOverlayService().GetOverlayApp();
    if (!pOverlay)
        return;

    auto pArgs = CefListValue::Create();
    pArgs->SetBool(0, m_session.Active);
    pArgs->SetInt(1, m_session.PartnerId);
    pArgs->SetBool(2, m_session.InitiatedBySelf);
    pArgs->SetBool(3, m_session.SelfReady);
    pArgs->SetBool(4, m_session.PartnerReady);

    auto& modSystem = m_world.GetModSystem();

    auto makeDict = [&](const Inventory::Entry& entry) {
        auto dict = CefDictionaryValue::Create();
        dict->SetInt("modId", entry.BaseId.ModId);
        dict->SetInt("baseId", entry.BaseId.BaseId);
        dict->SetBool("isQuestItem", entry.IsQuestItem);
        dict->SetBool("isEquipped", entry.IsWorn());
        dict->SetString("name", MakeDisplayName(modSystem, entry));
        dict->SetBool("isGold", entry.BaseId.ModId == 0 && entry.BaseId.BaseId == 0x0000000F);

        auto details = CefListValue::Create();
        int detailIndex = 0;
        if (entry.ExtraHealth > 1.0f)
            details->SetString(detailIndex++, fmt::format("Improvement: {:.0f}%", entry.ExtraHealth * 100.0f));
        if (entry.ExtraEnchantId.BaseId || !entry.EnchantData.Effects.empty())
            details->SetString(detailIndex++, "Enchanted");
        if (entry.ExtraPoisonId.BaseId)
            details->SetString(detailIndex++, "Poisoned");
        if (entry.ExtraSoulLevel > 0)
            details->SetString(detailIndex++, fmt::format("Soul level: {}", entry.ExtraSoulLevel));
        dict->SetList("details", details);
        return dict;
    };

    std::vector<int32_t> usedCounts(m_session.SelfInventory.size(), 0);

    auto selfList = CefListValue::Create();
    for (size_t i = 0; i < m_session.SelfItems.size(); ++i)
    {
        const auto& item = m_session.SelfItems[i];
        auto dict = makeDict(item);
        dict->SetInt("count", std::abs(item.Count));

        std::optional<uint32_t> match;
        const auto needed = std::abs(item.Count);
        for (uint32_t idx = 0; idx < m_session.SelfInventory.size(); ++idx)
        {
            const auto& inventoryEntry = m_session.SelfInventory[idx];
            if (!SameTradeItem(inventoryEntry, item))
                continue;

            const int32_t available = inventoryEntry.Count;
            const int32_t alreadyUsed = usedCounts[idx];
            if (alreadyUsed + needed > available)
                continue;

            match = idx;
            usedCounts[idx] += needed;
            break;
        }

        if (match)
            dict->SetInt("inventoryIndex", static_cast<int>(*match));

        selfList->SetDictionary(static_cast<int>(i), dict);
    }
    pArgs->SetList(5, selfList);

    auto partnerList = CefListValue::Create();
    for (size_t i = 0; i < m_session.PartnerItems.size(); ++i)
    {
        const auto& item = m_session.PartnerItems[i];
        auto dict = makeDict(item);
        dict->SetInt("count", std::abs(item.Count));
        partnerList->SetDictionary(static_cast<int>(i), dict);
    }
    pArgs->SetList(6, partnerList);

    auto inventoryList = CefListValue::Create();
    for (size_t i = 0; i < m_session.SelfInventory.size(); ++i)
    {
        const auto& entry = m_session.SelfInventory[i];
        auto dict = makeDict(entry);
        dict->SetInt("count", entry.Count);
        dict->SetInt("inventoryIndex", static_cast<int>(i));
        dict->SetInt("offeredCount", i < usedCounts.size() ? usedCounts[i] : 0);
        inventoryList->SetDictionary(static_cast<int>(i), dict);
    }
    pArgs->SetList(7, inventoryList);
    pArgs->SetInt(8, static_cast<int>(m_session.CountdownMs));
    pArgs->SetInt(9, static_cast<int>(m_session.CountdownTotalMs));

    pOverlay->ExecuteAsync("tradeStateUpdated", pArgs);
}

void TradeService::EmitInviteUpdate(uint32_t aInviterId, bool aAdded, uint64_t aExpiryTick) const noexcept
{
    auto* pOverlay = m_world.GetOverlayService().GetOverlayApp();
    if (!pOverlay)
        return;

    auto pArgs = CefListValue::Create();
    pArgs->SetInt(0, aInviterId);
    if (aAdded)
    {
        pArgs->SetDouble(1, static_cast<double>(aExpiryTick));
        pOverlay->ExecuteAsync("tradeInviteReceived", pArgs);
    }
    else
    {
        pOverlay->ExecuteAsync("tradeInviteExpired", pArgs);
    }
}

void TradeService::EmitCancellation(uint32_t aPartnerId, TradeCancelReason aReason, bool aWasInitiator) const noexcept
{
    auto* pOverlay = m_world.GetOverlayService().GetOverlayApp();
    if (!pOverlay)
        return;

    auto pArgs = CefListValue::Create();
    pArgs->SetInt(0, aPartnerId);
    pArgs->SetInt(1, static_cast<int>(aReason));
    pArgs->SetBool(2, aWasInitiator);

    pOverlay->ExecuteAsync("tradeCancelled", pArgs);
}

bool TradeService::HasActiveSessionWith(uint32_t aPartnerId) const noexcept
{
    return m_session.Active && m_session.PartnerId == aPartnerId;
}
