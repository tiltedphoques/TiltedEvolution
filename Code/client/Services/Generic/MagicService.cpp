#include <Services/MagicService.h>

#include <World.h>

#include <Events/UpdateEvent.h>
#include <Events/SpellCastEvent.h>
#include <Events/InterruptCastEvent.h>
#include <Events/AddTargetEvent.h>
#include <Events/RemoveSpellEvent.h>

#include <Messages/RemoveSpellRequest.h>

#include <Messages/SpellCastRequest.h>
#include <Messages/InterruptCastRequest.h>

#include <Messages/NotifySpellCast.h>
#include <Messages/NotifyInterruptCast.h>

#include <Actor.h>
#include <Magic/ActorMagicCaster.h>
#include <Games/ActorExtension.h>
#include <EquipManager.h>

#include <Structs/Skyrim/AnimationGraphDescriptor_VampireLordBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_WerewolfBehavior.h>

#include <Games/Overrides.h>

#include <Forms/SpellItem.h>
#include <PlayerCharacter.h>

#include <Games/TES.h>

MagicService::MagicService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&MagicService::OnUpdate>(this);
    m_spellCastEventConnection = m_dispatcher.sink<SpellCastEvent>().connect<&MagicService::OnSpellCastEvent>(this);
    m_notifySpellCastConnection = m_dispatcher.sink<NotifySpellCast>().connect<&MagicService::OnNotifySpellCast>(this);
    m_interruptCastEventConnection = m_dispatcher.sink<InterruptCastEvent>().connect<&MagicService::OnInterruptCastEvent>(this);
    m_notifyInterruptCastConnection = m_dispatcher.sink<NotifyInterruptCast>().connect<&MagicService::OnNotifyInterruptCast>(this);
    m_addTargetEventConnection = m_dispatcher.sink<AddTargetEvent>().connect<&MagicService::OnAddTargetEvent>(this);
    m_notifyAddTargetConnection = m_dispatcher.sink<NotifyAddTarget>().connect<&MagicService::OnNotifyAddTarget>(this);
    m_removeSpellEventConnection = m_dispatcher.sink<RemoveSpellEvent>().connect<&MagicService::OnRemoveSpellEvent>(this);
    m_notifyRemoveSpell = m_dispatcher.sink<NotifyRemoveSpell>().connect<&MagicService::OnNotifyRemoveSpell>(this);
}

void MagicService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    ApplyQueuedEffects();

    UpdateRevealOtherPlayersEffect();
}

void MagicService::OnSpellCastEvent(const SpellCastEvent& acEvent) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    if (!acEvent.pCaster->pCasterActor || !acEvent.pCaster->pCasterActor->GetNiNode())
    {
        spdlog::warn("Spell cast event has no actor or actor is not loaded");
        return;
    }

    // only sync concentration spells through spell cast sync, the rest through projectile sync for accuracy
    if (SpellItem* pSpell = Cast<SpellItem>(TESForm::GetById(acEvent.SpellId)))
    {
        if ((pSpell->eCastingType != MagicSystem::CastingType::CONCENTRATION || pSpell->IsHealingSpell()) && !pSpell->IsWardSpell() && !pSpell->IsInvisibilitySpell())
        {
            spdlog::debug("Canceled magic spell");
            return;
        }
    }

    uint32_t formId = acEvent.pCaster->pCasterActor->formID;

    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto casterEntityIt = std::find_if(std::begin(view), std::end(view), [formId, view](entt::entity entity) { return view.get<FormIdComponent>(entity).Id == formId; });

    if (casterEntityIt == std::end(view))
        return;

    auto& localComponent = view.get<LocalComponent>(*casterEntityIt);

    SpellCastRequest request{};

    request.CasterId = localComponent.Id;
    request.CastingSource = acEvent.pCaster->GetCastingSource();
    request.IsDualCasting = acEvent.pCaster->GetIsDualCasting();

    if (!m_world.GetModSystem().GetServerModId(acEvent.SpellId, request.SpellFormId))
    {
        spdlog::error("Server spell id not found for spell form id {:X}", acEvent.SpellId);
        return;
    }

    if (acEvent.DesiredTargetID != 0)
    {
        auto targetView = m_world.view<FormIdComponent>();
        const auto targetEntityIt = std::find_if(std::begin(targetView), std::end(targetView), [id = acEvent.DesiredTargetID, targetView](entt::entity entity) { return targetView.get<FormIdComponent>(entity).Id == id; });

        if (targetEntityIt != std::end(targetView))
        {
            auto desiredTargetIdRes = Utils::GetServerId(*targetEntityIt);
            if (desiredTargetIdRes.has_value())
                request.DesiredTarget = desiredTargetIdRes.value();
            else
                spdlog::error("{}: failed to find server id", __FUNCTION__);
        }
    }

    spdlog::debug("Spell cast event sent, ID: {:X}, Source: {}, IsDualCasting: {}, desired target: {:X}", request.CasterId, request.CastingSource, request.IsDualCasting, request.DesiredTarget);

    m_transport.Send(request);
}

void MagicService::OnNotifySpellCast(const NotifySpellCast& acMessage) const noexcept
{
    using CS = MagicSystem::CastingSource;

    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.CasterId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Caster with remote id {:X} not found.", acMessage.CasterId);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);
    TESForm* pForm = TESForm::GetById(formIdComponent.Id);
    Actor* pActor = Cast<Actor>(pForm);

    pActor->GenerateMagicCasters();

    // Only left hand casters need dual casting (?)
    pActor->casters[CS::LEFT_HAND]->SetDualCasting(acMessage.IsDualCasting);

    if (acMessage.CastingSource >= 4)
    {
        spdlog::warn("{}: could not find casting source {}", __FUNCTION__, acMessage.CastingSource);
        return;
    }

    MagicItem* pSpell = nullptr;

    pSpell = pActor->magicItems[acMessage.CastingSource];

    if (!pSpell)
    {
        const uint32_t cSpellFormId = World::Get().GetModSystem().GetGameId(acMessage.SpellFormId);
        if (cSpellFormId == 0)
        {
            spdlog::error("Could not find spell form id for GameId base {:X}, mod {:X}", acMessage.SpellFormId.BaseId, acMessage.SpellFormId.ModId);
            return;
        }

        TESForm* pSpellForm = TESForm::GetById(cSpellFormId);
        if (!pSpellForm)
        {
            spdlog::error("Cannot find spell form, id: {:X}.", cSpellFormId);
            return;
        }

        pSpell = Cast<MagicItem>(pSpellForm);
    }

    if (!pSpell)
    {
        spdlog::error("Could not find spell.");
        return;
    }

    TESObjectREFR* pDesiredTarget = nullptr;

    if (acMessage.DesiredTarget != 0)
    {
        auto view = m_world.view<FormIdComponent>();
        for (auto entity : view)
        {
            std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
            if (!serverIdRes.has_value())
            {
                spdlog::error("{}: failed to find server id", __FUNCTION__);
                continue;
            }

            uint32_t serverId = serverIdRes.value();

            if (serverId == acMessage.DesiredTarget)
            {
                const auto& formIdComponent = view.get<FormIdComponent>(entity);
                pDesiredTarget = Cast<TESObjectREFR>(TESForm::GetById(formIdComponent.Id));
            }
        }
    }

    ScopedSpellCastOverride _;

    MagicCaster* pCaster = pActor->GetMagicCaster(static_cast<CS>(acMessage.CastingSource));
    if (!pCaster)
    {
        spdlog::warn("{}: failed to find caster.", __FUNCTION__);
        return;
    }

    pCaster->CastSpellImmediate(pSpell, false, pDesiredTarget, 1.0f, false, 0.0f);

    spdlog::debug("Successfully casted remote spell");
}

void MagicService::OnInterruptCastEvent(const InterruptCastEvent& acEvent) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    uint32_t formId = acEvent.CasterFormID;

    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto casterEntityIt = std::find_if(std::begin(view), std::end(view), [formId, view](entt::entity entity) { return view.get<FormIdComponent>(entity).Id == formId; });

    if (casterEntityIt == std::end(view))
    {
        spdlog::warn("{}: could not find caster, form id {:X}", __FUNCTION__, formId);
        return;
    }

    auto& localComponent = view.get<LocalComponent>(*casterEntityIt);

    InterruptCastRequest request;
    request.CasterId = localComponent.Id;
    request.CastingSource = acEvent.CastingSource;

    spdlog::debug("Sending out interrupt cast");

    m_transport.Send(request);
}

void MagicService::OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept
{
    if (acMessage.CastingSource >= 4)
    {
        spdlog::warn("{}: could not find casting source {}", __FUNCTION__, acMessage.CastingSource);
        return;
    }

    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.CasterId](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Caster with remote id {:X} not found.", acMessage.CasterId);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);

    const TESForm* pForm = TESForm::GetById(formIdComponent.Id);
    Actor* pActor = Cast<Actor>(pForm);

    pActor->GenerateMagicCasters();

    MagicCaster* pCaster = pActor->GetMagicCaster(static_cast<MagicSystem::CastingSource>(acMessage.CastingSource));
    if (!pCaster)
    {
        spdlog::warn("{}: failed to find caster.", __FUNCTION__);
        return;
    }

    pCaster->InterruptCast();

    spdlog::debug("Interrupt remote cast successful");
}

void MagicService::OnAddTargetEvent(const AddTargetEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    // These effects are applied through spell cast sync
    if (SpellItem* pSpellItem = Cast<SpellItem>(TESForm::GetById(acEvent.SpellID)))
    {
        if ((pSpellItem->eCastingType == MagicSystem::CastingType::CONCENTRATION && !pSpellItem->IsHealingSpell()) || pSpellItem->IsWardSpell() || pSpellItem->IsInvisibilitySpell() || pSpellItem->IsBoundWeaponSpell())
        {
            return;
        }
    }

    AddTargetRequest request{};

    if (!m_world.GetModSystem().GetServerModId(acEvent.SpellID, request.SpellId.ModId, request.SpellId.BaseId))
    {
        spdlog::error("{}: Could not find spell with form {:X}", __FUNCTION__, acEvent.SpellID);
        return;
    }

    if (!m_world.GetModSystem().GetServerModId(acEvent.EffectID, request.EffectId.ModId, request.EffectId.BaseId))
    {
        spdlog::error("{}: Could not find effect with form {:X}", __FUNCTION__, acEvent.EffectID);
        return;
    }

    request.Magnitude = acEvent.Magnitude;

    auto view = m_world.view<FormIdComponent>();
    const auto it = std::find_if(std::begin(view), std::end(view), [id = acEvent.TargetID, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (it == std::end(view))
    {
        spdlog::warn("Form id not found for magic add target, form id: {:X}", acEvent.TargetID);
        m_queuedEffects[acEvent.TargetID] = request;
        return;
    }

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(*it);
    if (!serverIdRes.has_value())
    {
        spdlog::warn("Server id not found for magic add target, form id: {:X}", acEvent.TargetID);
        m_queuedEffects[acEvent.TargetID] = request;
        return;
    }

    request.TargetId = serverIdRes.value();

    const auto casterIt = std::find_if(std::begin(view), std::end(view), [id = acEvent.CasterID, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (casterIt == std::end(view))
    {
        spdlog::warn("Form id not found for magic add target, form id: {:X}", acEvent.CasterID);
        m_queuedEffects[acEvent.TargetID] = request;
        return;
    }

    serverIdRes = Utils::GetServerId(*casterIt);
    if (serverIdRes.has_value())
        request.CasterId = serverIdRes.value();

    request.IsDualCasting = acEvent.IsDualCasting;
    request.ApplyHealPerkBonus = acEvent.ApplyHealPerkBonus;
    request.ApplyStaminaPerkBonus = acEvent.ApplyStaminaPerkBonus;

    m_transport.Send(request);

    spdlog::debug("Sending effect sync request");
}

void MagicService::OnNotifyAddTarget(const NotifyAddTarget& acMessage) noexcept
{
    Actor* pActor = Utils::GetByServerId<Actor>(acMessage.TargetId);
    if (!pActor)
    {
        spdlog::warn(__FUNCTION__ ": could not find actor server id {:X}", acMessage.TargetId);
        m_queuedRemoteEffects[acMessage.TargetId] = acMessage;
        return;
    }

    const uint32_t cSpellId = World::Get().GetModSystem().GetGameId(acMessage.SpellId);
    if (cSpellId == 0)
    {
        spdlog::error("{}: failed to retrieve spell id, GameId base: {:X}, mod: {:X}", __FUNCTION__, acMessage.SpellId.BaseId, acMessage.SpellId.ModId);
        return;
    }

    MagicItem* pSpell = Cast<MagicItem>(TESForm::GetById(cSpellId));
    if (!pSpell)
    {
        spdlog::error("{}: Failed to retrieve spell by id {:X}", __FUNCTION__, cSpellId);
        return;
    }

    const uint32_t cEffectId = World::Get().GetModSystem().GetGameId(acMessage.EffectId);
    if (cEffectId == 0)
    {
        spdlog::error("{}: failed to retrieve effect id, GameId base: {:X}, mod: {:X}", __FUNCTION__, acMessage.EffectId.BaseId, acMessage.EffectId.ModId);
        return;
    }

    EffectItem* pEffect = pSpell->GetEffect(cEffectId);

    if (!pEffect)
    {
        spdlog::error("{}: Failed to retrieve effect by id {:X}", __FUNCTION__, cEffectId);
        return;
    }

    MagicTarget::AddTargetData data{};
    data.pSpell = pSpell;
    data.pEffectItem = pEffect;
    data.fMagnitude = acMessage.Magnitude;
    data.fUnkFloat1 = 1.0f;
    data.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;
    data.bDualCast = acMessage.IsDualCasting;

    if (pEffect->IsWerewolfEffect())
        pActor->GetExtension()->GraphDescriptorHash = AnimationGraphDescriptor_WerewolfBehavior::m_key;

    if (pEffect->IsVampireLordEffect())
        pActor->GetExtension()->GraphDescriptorHash = AnimationGraphDescriptor_VampireLordBehavior::m_key;

    // This hack is here because slow time seems to be twice as slow when cast by an npc
    if (pEffect->IsSlowEffect())
        pActor = PlayerCharacter::Get();

    data.pCaster = Utils::GetByServerId<Actor>(acMessage.CasterId);

    pActor->magicTarget.AddTarget(data, acMessage.ApplyHealPerkBonus, acMessage.ApplyStaminaPerkBonus);

    spdlog::debug("Applied remote magic effect");
}

void MagicService::OnRemoveSpellEvent(const RemoveSpellEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    RemoveSpellRequest request{};

    if (!m_world.GetModSystem().GetServerModId(acEvent.SpellId, request.SpellId.ModId, request.SpellId.BaseId))
    {
        spdlog::error("{}: Could not find spell with form {:X}", __FUNCTION__, acEvent.SpellId);
        return;
    }

    auto view = m_world.view<FormIdComponent>();
    const auto it = std::find_if(std::begin(view), std::end(view), [id = acEvent.TargetId, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (it == std::end(view))
    {
        spdlog::warn("Form id not found for magic remove target, form id: {:X}", acEvent.TargetId);
        return;
    }

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(*it);
    if (!serverIdRes.has_value())
    {
        spdlog::warn("Server id not found for magic remove target, form id: {:X}", acEvent.TargetId);
        return;
    }

    request.TargetId = serverIdRes.value();

    // spdlog::info(__FUNCTION__ ": requesting remove spell with base id {:X} from actor with server id {:X}", request.SpellId.BaseId, request.TargetId);

    m_transport.Send(request);
}

void MagicService::OnNotifyRemoveSpell(const NotifyRemoveSpell& acMessage) noexcept
{
    uint32_t targetFormId = acMessage.TargetId;

    Actor* pActor = Utils::GetByServerId<Actor>(acMessage.TargetId);
    if (!pActor)
    {
        spdlog::warn(__FUNCTION__ ": could not find actor server id {:X}", acMessage.TargetId);
        return;
    }

    const uint32_t cSpellId = World::Get().GetModSystem().GetGameId(acMessage.SpellId);
    if (cSpellId == 0)
    {
        spdlog::error("{}: failed to retrieve spell id, GameId base: {:X}, mod: {:X}", __FUNCTION__, acMessage.SpellId.BaseId, acMessage.SpellId.ModId);
        return;
    }

    MagicItem* pSpell = Cast<MagicItem>(TESForm::GetById(cSpellId));
    if (!pSpell)
    {
        spdlog::error("{}: Failed to retrieve spell by id {:X}", __FUNCTION__, cSpellId);
        return;
    }

    // Remove the spell from the actor
    // spdlog::info(__FUNCTION__ ": removing spell with form id {:X} from actor with form id {:X}", cSpellId, targetFormId);
    pActor->RemoveSpell(pSpell);
}

void MagicService::ApplyQueuedEffects() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenUpdates = 100ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    Vector<uint32_t> markedForRemoval{};

    for (auto [formId, request] : m_queuedEffects)
    {
        auto view = m_world.view<FormIdComponent>();
        const auto it = std::find_if(std::begin(view), std::end(view), [id = formId, view](auto entity) { return view.get<FormIdComponent>(entity).Id == id; });

        if (it == std::end(view))
            continue;

        entt::entity entity = *it;

        std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
        if (!serverIdRes.has_value())
            continue;

        request.TargetId = serverIdRes.value();

        m_transport.Send(request);

        markedForRemoval.push_back(formId);
    }

    for (uint32_t formId : markedForRemoval)
        m_queuedEffects.erase(formId);

    markedForRemoval.clear();

    for (const auto& [serverId, notify] : m_queuedRemoteEffects)
    {
        Actor* pActor = Utils::GetByServerId<Actor>(serverId);
        if (!pActor)
            continue;

        OnNotifyAddTarget(notify);

        markedForRemoval.push_back(serverId);
    }

    for (uint32_t serverId : markedForRemoval)
        m_queuedRemoteEffects.erase(serverId);
}

void MagicService::StartRevealingOtherPlayers() noexcept
{
    UpdateRevealOtherPlayersEffect(/*forceTrigger=*/true);
}

void MagicService::UpdateRevealOtherPlayersEffect(bool aForceTrigger) noexcept
{
    constexpr auto cRevealDuration = 10s;
    constexpr auto cDelayBetweenUpdates = 2s;

    // Effect's activation and lifecycle

    static std::chrono::steady_clock::time_point revealStartTimePoint;
    static std::chrono::steady_clock::time_point lastSendTimePoint;

    const bool shouldActivate = aForceTrigger || m_revealKeybindPressed;

    if (shouldActivate && !m_revealingOtherPlayers)
    {
        m_revealingOtherPlayers = true;
        revealStartTimePoint = std::chrono::steady_clock::now();
    }

    if (!m_revealingOtherPlayers)
        return;

    const auto now = std::chrono::steady_clock::now();

    if (now - revealStartTimePoint > cRevealDuration)
    {
        m_revealingOtherPlayers = false;
        return;
    }

    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    // When active

    Mod* pSkyrimTogether = ModManager::Get()->GetByName("SkyrimTogether.esp");
    if (!pSkyrimTogether)
        return;

    MagicItem* pSpell = Cast<MagicItem>(TESForm::GetById((pSkyrimTogether->standardId << 24) | 0x1825));

    if (!pSpell)
        return;

    MagicTarget::AddTargetData data{};
    data.pSpell = pSpell;
    data.pEffectItem = pSpell->GetEffect((pSkyrimTogether->standardId << 24) | 0x1824);
    data.fMagnitude = 1.f;
    data.fUnkFloat1 = 1.f;
    data.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;

    auto view = World::Get().view<FormIdComponent, PlayerComponent>();
    for (const auto entity : view)
    {
        auto& formIdComponent = view.get<FormIdComponent>(entity);
        if (formIdComponent.Id == 0x14)
            continue;

        auto* pRemotePlayer = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
        if (!pRemotePlayer)
            continue;

        pRemotePlayer->magicTarget.AddTarget(data, false, false);
    }
}

void MagicService::RevealKeybindPressed() noexcept
{
    m_revealKeybindPressed = !m_revealKeybindPressed;

    UpdateRevealOtherPlayersEffect(m_revealKeybindPressed);

    m_revealKeybindPressed = !m_revealKeybindPressed;
}
