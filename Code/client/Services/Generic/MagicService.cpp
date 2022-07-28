#include <Services/MagicService.h>

#include <World.h>

#include <Events/UpdateEvent.h>
#include <Events/SpellCastEvent.h>
#include <Events/InterruptCastEvent.h>
#include <Events/AddTargetEvent.h>

#include <Messages/SpellCastRequest.h>
#include <Messages/InterruptCastRequest.h>

#include <Messages/NotifySpellCast.h>
#include <Messages/NotifyInterruptCast.h>
#include <Messages/NotifyAddTarget.h>

#include <Actor.h>
#include <Magic/ActorMagicCaster.h>
#include <Games/ActorExtension.h>

#include <Structs/Skyrim/AnimationGraphDescriptor_VampireLordBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_WerewolfBehavior.h>

#include <Games/Overrides.h>

#include <Forms/SpellItem.h>
#include <PlayerCharacter.h>

// TODO: these paths are inconsistent
#if TP_FALLOUT4
#include <Magic/EffectItem.h>
#include <Magic/EffectSetting.h>
#endif

// TODO: ft
MagicService::MagicService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept 
    : m_world(aWorld), m_dispatcher(aDispatcher), m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&MagicService::OnUpdate>(this);
    m_spellCastEventConnection = m_dispatcher.sink<SpellCastEvent>().connect<&MagicService::OnSpellCastEvent>(this);
    m_notifySpellCastConnection = m_dispatcher.sink<NotifySpellCast>().connect<&MagicService::OnNotifySpellCast>(this);
    m_interruptCastEventConnection = m_dispatcher.sink<InterruptCastEvent>().connect<&MagicService::OnInterruptCastEvent>(this);
    m_notifyInterruptCastConnection = m_dispatcher.sink<NotifyInterruptCast>().connect<&MagicService::OnNotifyInterruptCast>(this);
    m_addTargetEventConnection = m_dispatcher.sink<AddTargetEvent>().connect<&MagicService::OnAddTargetEvent>(this);
    m_notifyAddTargetConnection = m_dispatcher.sink<NotifyAddTarget>().connect<&MagicService::OnNotifyAddTarget>(this);
}

void MagicService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    ApplyQueuedEffects();
}

void MagicService::OnSpellCastEvent(const SpellCastEvent& acEvent) const noexcept
{
#if TP_SKYRIM64
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
        if ((pSpell->eCastingType != MagicSystem::CastingType::CONCENTRATION ||
             pSpell->IsHealingSpell()) &&
            !pSpell->IsWardSpell() &&
            !pSpell->IsInvisibilitySpell())
        {
            spdlog::debug("Canceled magic spell");
            return;
        }
    }

    uint32_t formId = acEvent.pCaster->pCasterActor->formID;

    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto casterEntityIt = std::find_if(std::begin(view), std::end(view), [formId, view](entt::entity entity)
    {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

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
        const auto targetEntityIt = std::find_if(std::begin(targetView), std::end(targetView), [id = acEvent.DesiredTargetID, targetView](entt::entity entity)
        {
            return targetView.get<FormIdComponent>(entity).Id == id;
        });

        if (targetEntityIt != std::end(targetView))
        {
            auto desiredTargetIdRes = Utils::GetServerId(*targetEntityIt);
            if (desiredTargetIdRes.has_value())
                request.DesiredTarget = desiredTargetIdRes.value();
            else
                spdlog::error("{}: failed to find server id", __FUNCTION__);
        }
    }

    spdlog::debug("Spell cast event sent, ID: {:X}, Source: {}, IsDualCasting: {}, desired target: {:X}", request.CasterId,
                 request.CastingSource, request.IsDualCasting, request.DesiredTarget);

    m_transport.Send(request);
#endif
}

void MagicService::OnNotifySpellCast(const NotifySpellCast& acMessage) const noexcept
{
#if TP_SKYRIM64
    using CS = MagicSystem::CastingSource;

    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.CasterId](auto entity)
    {
        return remoteView.get<RemoteComponent>(entity).Id == Id;
    });

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
            spdlog::error("Could not find spell form id for GameId base {:X}, mod {:X}", acMessage.SpellFormId.BaseId,
                          acMessage.SpellFormId.ModId);
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
#endif
}

void MagicService::OnInterruptCastEvent(const InterruptCastEvent& acEvent) const noexcept
{
#if TP_SKYRIM64
    if (!m_transport.IsConnected())
        return;

    uint32_t formId = acEvent.CasterFormID;

    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto casterEntityIt = std::find_if(std::begin(view), std::end(view), [formId, view](entt::entity entity)
    {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

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
#endif
}

void MagicService::OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept
{
#if TP_SKYRIM64
    if (acMessage.CastingSource >= 4)
    {
        spdlog::warn("{}: could not find casting source {}", __FUNCTION__, acMessage.CastingSource);
        return;
    }

    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.CasterId](auto entity)
    {
        return remoteView.get<RemoteComponent>(entity).Id == Id;
    });

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
#endif
}

void MagicService::OnAddTargetEvent(const AddTargetEvent& acEvent) noexcept
{
#if 1
    if (!m_transport.IsConnected())
        return;

#if TP_SKYRIM64
    // These effects are applied through spell cast sync
    if (SpellItem* pSpellItem = Cast<SpellItem>(TESForm::GetById(acEvent.SpellID)))
    {
        if ((pSpellItem->eCastingType == MagicSystem::CastingType::CONCENTRATION && 
             !pSpellItem->IsHealingSpell()) || 
            pSpellItem->IsWardSpell() ||
            pSpellItem->IsInvisibilitySpell())
        {
            return;
        }
    }
#endif

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
    const auto it = std::find_if(std::begin(view), std::end(view), [id = acEvent.TargetID, view](auto entity) {
        return view.get<FormIdComponent>(entity).Id == id;
    });

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
    m_transport.Send(request);

    spdlog::debug("Sending effect sync request");
#endif
}

void MagicService::OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept
{
    Actor* pActor = Utils::GetByServerId<Actor>(acMessage.TargetId);
    if (!pActor)
    {
        spdlog::error("{}: could not find actor server id {:X}", __FUNCTION__, acMessage.TargetId);
        return;
    }

    const uint32_t cSpellId = World::Get().GetModSystem().GetGameId(acMessage.SpellId);
    if (cSpellId == 0)
    {
        spdlog::error("{}: failed to retrieve spell id, GameId base: {:X}, mod: {:X}", __FUNCTION__,
                      acMessage.SpellId.BaseId, acMessage.SpellId.ModId);
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
        spdlog::error("{}: failed to retrieve effect id, GameId base: {:X}, mod: {:X}", __FUNCTION__,
                      acMessage.EffectId.BaseId, acMessage.EffectId.ModId);
        return;
    }

    EffectItem* pEffect = nullptr;

    for (EffectItem* effect : pSpell->listOfEffects)
    {
        if (effect->pEffectSetting->formID == cEffectId)
        {
            pEffect = effect;
            break;
        }
    }

    if (!pEffect)
    {
        spdlog::error("{}: Failed to retrieve effect by id {:X}", __FUNCTION__, cEffectId);
        return;
    }

    MagicTarget::AddTargetData data{};
    data.pSpell = pSpell;
    data.pEffectItem = pEffect;
    data.fMagnitude = acMessage.Magnitude;
#if TP_SKYRIM64
    data.fUnkFloat1 = 1.0f;
#endif
    data.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;

#if TP_SKYRIM64
    if (pEffect->IsWerewolfEffect())
        pActor->GetExtension()->GraphDescriptorHash = AnimationGraphDescriptor_WerewolfBehavior::m_key;

    if (pEffect->IsVampireLordEffect())
        pActor->GetExtension()->GraphDescriptorHash = AnimationGraphDescriptor_VampireLordBehavior::m_key;

    // TODO: ft, check if this bug also occurs in fallout 4
    // This hack is here because slow time seems to be twice as slow when cast by an npc
    if (pEffect->IsSlowEffect())
        pActor = PlayerCharacter::Get();
#endif

    pActor->magicTarget.AddTarget(data);

    spdlog::debug("Applied remote magic effect");
}

void MagicService::ApplyQueuedEffects() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenUpdates = 100ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    Vector<uint32_t> markedForRemoval;

    for (auto [formId, request] : m_queuedEffects)
    {
        auto view = m_world.view<FormIdComponent>();
        const auto it = std::find_if(std::begin(view), std::end(view), [id = formId, view](auto entity) {
            return view.get<FormIdComponent>(entity).Id == id;
        });

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
}
