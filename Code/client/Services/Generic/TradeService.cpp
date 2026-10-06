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
#include <Games/Skyrim/Forms/AlchemyItem.h>
#include <Games/Skyrim/PlayerCharacter.h>
#include <Games/Skyrim/ExtraData/ExtraContainerChanges.h>
#include <Games/Skyrim/ExtraData/ExtraTextDisplayData.h>

#include <spdlog/fmt/fmt.h>

#include <cmath>
#include <algorithm>
#include <cstring>
#include <string>
#include <optional>
#include <vector>

namespace
{
constexpr uint64_t kInviteCleanupIntervalMs = 1000;

const char* GetItemCategory(TESForm* apForm) noexcept
{
    if (!apForm)
        return "misc";
    switch (apForm->formType)
    {
    case FormType::Weapon: return "weapons";
    case FormType::Ammo: return "ammunition";
    case FormType::Armor: return "armor";
    case FormType::Scroll: return "scrolls";
    case FormType::Ingredient: return "ingredients";
    case FormType::Book:
    case FormType::Note: return "books";
    case FormType::Key: return "keys";
    case FormType::SoulGem: return "soul_gems";
    case FormType::Alchemy:
        if (const auto* pAlchemy = Cast<AlchemyItem>(apForm))
        {
            if (pAlchemy->IsPoison())
                return "poisons";
            if (pAlchemy->IsFood())
                return "food";
        }
        return "potions";
    default: return "misc";
    }
}

bool EffectsResolve(ModSystem& aModSystem, const Vector<Inventory::EffectItem>& acEffects) noexcept
{
    for (const auto& effect : acEffects)
        if (!Cast<EffectSetting>(TESForm::GetById(aModSystem.GetGameId(effect.EffectId))))
            return false;
    return true;
}

// Whether this game can rebuild a partner's item: every plugin form it references must be loaded,
// and temporary forms must carry data to recreate them here.
bool CanReceiveTradeItem(ModSystem& aModSystem, const Inventory::Entry& acItem) noexcept
{
    if (!acItem.Potion.Effects.empty())
    {
        if (!acItem.Potion.IsValid() || !EffectsResolve(aModSystem, acItem.Potion.Effects))
            return false;
    }
    else if (acItem.BaseId.ModId == 0xFFFFFFFF || !Cast<TESBoundObject>(TESForm::GetById(aModSystem.GetGameId(acItem.BaseId))))
        return false;

    if (acItem.ExtraEnchantId != 0)
    {
        if (acItem.ExtraEnchantId.ModId == 0xFFFFFFFF)
        {
            // Player-crafted enchantment: recreated from its effects on the recipient.
            if (acItem.EnchantData.Effects.empty() || !EffectsResolve(aModSystem, acItem.EnchantData.Effects))
                return false;
        }
        else if (!TESForm::GetById(aModSystem.GetGameId(acItem.ExtraEnchantId)))
            return false;
    }

    if (acItem.ExtraPoisonId != 0)
    {
        // Applied crafted poisons carry no recipe in the inventory entry.
        if (acItem.ExtraPoisonId.ModId == 0xFFFFFFFF || !Cast<AlchemyItem>(TESForm::GetById(aModSystem.GetGameId(acItem.ExtraPoisonId))))
            return false;
    }
    return true;
}

std::string MakeDisplayName(ModSystem& aModSystem, const Inventory::Entry& aEntry)
{
    if (!aEntry.Potion.Effects.empty())
    {
        auto* local = AlchemyItem::Find(PlayerCharacter::Get(), aEntry.Potion);
        auto* created = local ? nullptr : AlchemyItem::Create(aEntry.Potion);
        auto* potion = local ? local : created;
        std::string name = potion && potion->GetName() ? potion->GetName() : "Crafted potion (unavailable)";
        AlchemyItem::Release(created);
        return name;
    }
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

// Names are local UI metadata only: offers and network inventory retain the base name.
CefRefPtr<CefListValue> GetLocalCustomNames(ModSystem& aModSystem, const Inventory::Entry& aEntry)
{
    auto names = CefListValue::Create();
    auto* player = PlayerCharacter::Get();
    auto* changes = player ? player->GetContainerChanges() : nullptr;
    if (!changes || !changes->entries)
        return names;

    const auto* potion = aEntry.Potion.Effects.empty() ? nullptr : AlchemyItem::Find(player, aEntry.Potion);
    const auto formId = potion ? potion->formID : aModSystem.GetGameId(aEntry.BaseId);
    std::vector<std::string> seen;
    for (auto* entry : *changes->entries)
    {
        if (!entry || !entry->form || entry->form->formID != formId || !entry->dataList)
            continue;
        for (auto* extra : *entry->dataList)
        {
            if (!extra)
                continue;
            auto* text = Cast<ExtraTextDisplayData>(extra->GetByType(ExtraDataType::TextDisplayData));
            const char* name = text ? text->DisplayName.AsAscii() : nullptr;
            if (!name || !*name)
                continue;
            Inventory::Entry local;
            local.BaseId = aEntry.BaseId;
            local.Potion = aEntry.Potion;
            TESObjectREFR::GetItemFromExtraData(local, extra);
            if (!SameTradeItem(local, aEntry) || std::find(seen.begin(), seen.end(), name) != seen.end())
                continue;
            seen.emplace_back(name);
            names->SetString(static_cast<int>(seen.size() - 1), name);
        }
    }
    return names;
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

TradeService::~TradeService() noexcept
{
    ReleasePreparedPotions();
}

void TradeService::ReleasePreparedPotions() noexcept
{
    for (auto* potion : m_preparedPotions)
        AlchemyItem::Release(potion);
    m_preparedPotions.clear();
}

void TradeService::SetReady(bool aReady) noexcept
{
    TradeSetReadyRequest request{};
    request.Ready = aReady;
    if (aReady)
    {
        auto* player = PlayerCharacter::Get();
        if (!player || !ValidateTradeOffer(player->GetInventory(), m_session.SelfItems))
        {
            CancelTrade();
            return;
        }
        // Accept only partner items this game can rebuild; the rest stay with the partner.
        // Keep prepared potion forms alive through cancellation races until the session ends.
        auto& modSystem = m_world.GetModSystem();
        for (const auto& item : m_session.PartnerItems)
        {
            if (!CanReceiveTradeItem(modSystem, item))
                continue;
            if (!item.Potion.Effects.empty())
            {
                auto* created = AlchemyItem::Create(item.Potion);
                if (!created)
                {
                    spdlog::error("[TradeService]: Unable to prepare incoming crafted potion; leaving it with the partner");
                    continue;
                }
                if (std::find(m_preparedPotions.begin(), m_preparedPotions.end(), created) == m_preparedPotions.end())
                    m_preparedPotions.push_back(created);
                else
                    AlchemyItem::Release(created);
            }
            request.AcceptedItems.push_back(item);
        }
    }
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
    m_session.SelfAcceptedItems.clear();
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
    m_session.SelfAcceptedItems = acMessage.SelfAcceptedItems;
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
    ReleasePreparedPotions();
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

    auto makeDict = [&](const Inventory::Entry& entry, const char* apNote = nullptr) {
        auto dict = CefDictionaryValue::Create();
        dict->SetInt("modId", entry.BaseId.ModId);
        dict->SetInt("baseId", entry.BaseId.BaseId);
        dict->SetBool("isQuestItem", entry.IsQuestItem);
        dict->SetBool("isEquipped", entry.IsWorn());
        dict->SetBool("isUnsupportedTemporary", entry.BaseId.ModId == 0xFFFFFFFF && !entry.Potion.IsValid());
        dict->SetString("name", MakeDisplayName(modSystem, entry));
        dict->SetBool("isGold", entry.BaseId.ModId == 0 && entry.BaseId.BaseId == 0x0000000F);
        const uint32_t formId = modSystem.GetGameId(entry.BaseId);
        dict->SetString("category", !entry.Potion.Effects.empty() ? (entry.Potion.IsPoison ? "poisons" : "potions") : GetItemCategory(formId ? TESForm::GetById(formId) : nullptr));

        auto details = CefListValue::Create();
        int detailIndex = 0;
        if (!entry.Potion.Effects.empty())
        {
            // Temporary IDs belong to the owner's game session; the recipient gets a fresh one from AddPotion.
            details->SetString(detailIndex++, fmt::format("Temporary ID: {:08X}", 0xFF000000u | (entry.BaseId.BaseId & 0x00FFFFFFu)));
            for (const auto& effect : entry.Potion.Effects)
            {
                auto* form = TESForm::GetById(modSystem.GetGameId(effect.EffectId));
                std::string text = fmt::format("{}: {:.0f}", form && form->GetName() ? form->GetName() : "Unknown effect", effect.Magnitude);
                if (effect.Area > 0)
                    text += fmt::format(", {} ft", effect.Area);
                if (effect.Duration > 0)
                    text += fmt::format(" for {}s", effect.Duration);
                details->SetString(detailIndex++, text);
            }
        }
        if (entry.ExtraHealth > 1.0f)
            details->SetString(detailIndex++, fmt::format("Improvement: {:.0f}%", entry.ExtraHealth * 100.0f));
        if (entry.ExtraEnchantId.BaseId || !entry.EnchantData.Effects.empty())
            details->SetString(detailIndex++, "Enchanted");
        if (entry.ExtraPoisonId.BaseId)
            details->SetString(detailIndex++, "Poisoned");
        if (entry.ExtraSoulLevel > 0)
            details->SetString(detailIndex++, fmt::format("Soul level: {}", entry.ExtraSoulLevel));
        if (apNote)
            details->SetString(detailIndex++, apNote);
        dict->SetList("details", details);
        return dict;
    };

    std::vector<int32_t> usedCounts(m_session.SelfInventory.size(), 0);
    const auto selfAccepted = MatchAcceptedItems(m_session.SelfItems, m_session.SelfAcceptedItems);

    auto selfList = CefListValue::Create();
    for (size_t i = 0; i < m_session.SelfItems.size(); ++i)
    {
        const auto& item = m_session.SelfItems[i];
        const bool notReceivable = m_session.PartnerReady && !selfAccepted[i];
        auto dict = makeDict(item, notReceivable ? "Partner can't receive this item. It stays with you." : nullptr);
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

    // Partner items this game cannot rebuild are hidden; they are never accepted and stay with the partner.
    auto partnerList = CefListValue::Create();
    int partnerIndex = 0;
    for (const auto& item : m_session.PartnerItems)
    {
        if (!CanReceiveTradeItem(modSystem, item))
            continue;
        auto dict = makeDict(item);
        dict->SetInt("count", std::abs(item.Count));
        partnerList->SetDictionary(partnerIndex++, dict);
    }
    pArgs->SetList(6, partnerList);

    auto inventoryList = CefListValue::Create();
    for (size_t i = 0; i < m_session.SelfInventory.size(); ++i)
    {
        const auto& entry = m_session.SelfInventory[i];
        auto dict = makeDict(entry);
        dict->SetInt("count", entry.Count);
        dict->SetInt("inventoryIndex", static_cast<int>(i));
        dict->SetList("customNames", GetLocalCustomNames(modSystem, entry));
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
