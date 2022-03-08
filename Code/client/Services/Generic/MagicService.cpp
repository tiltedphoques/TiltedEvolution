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
#if TP_SKYRIM64
#include <Forms/SpellItem.h>
#include <PlayerCharacter.h>
#endif

#define MAGIC_DEBUG 0

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

#if MAGIC_DEBUG
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->magicEffectApplyEvent.RegisterSink(this);
    pEventList->activeEffectApplyRemove.RegisterSink(this);
#endif
}

void MagicService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
#if TP_SKYRIM64
    if (!m_transport.IsConnected())
        return;

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
#endif
}

void MagicService::OnSpellCastEvent(const SpellCastEvent& acSpellCastEvent) const noexcept
{
#if TP_SKYRIM64
    if (!m_transport.IsConnected())
        return;

    TP_ASSERT(acSpellCastEvent.pSpell, "SpellCastEvent has no spell");

    if (!acSpellCastEvent.pCaster->pCasterActor || !acSpellCastEvent.pCaster->pCasterActor->GetNiNode())
    {
        spdlog::warn("Spell cast event has no actor or actor is not loaded");
        return;
    }

    // only sync concentration spells through spell cast sync, the rest through projectile sync
    // TODO: not all fire and forget spells have a projectile (i.e. heal other)
    if (SpellItem* pSpell = RTTI_CAST(acSpellCastEvent.pSpell, MagicItem, SpellItem))
    {
        if (pSpell->eCastingType != MagicSystem::CastingType::CONCENTRATION)
        {
            spdlog::debug("Canceled magic spell");
            return;
        }
    }

    uint32_t formId = acSpellCastEvent.pCaster->pCasterActor->formID;

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
    request.CastingSource = acSpellCastEvent.pCaster->GetCastingSource();
    request.IsDualCasting = acSpellCastEvent.pCaster->GetIsDualCasting();
    m_world.GetModSystem().GetServerModId(acSpellCastEvent.pSpell->formID, request.SpellFormId.ModId,
                                          request.SpellFormId.BaseId);
    if (acSpellCastEvent.DesiredTargetID != 0)
    {
        auto targetView = m_world.view<FormIdComponent>();
        const auto targetEntityIt = std::find_if(std::begin(targetView), std::end(targetView), [id = acSpellCastEvent.DesiredTargetID, targetView](entt::entity entity)
        {
            return targetView.get<FormIdComponent>(entity).Id == id;
        });

        if (targetEntityIt != std::end(targetView))
        {
            auto desiredTargetIdRes = Utils::GetServerId(*targetEntityIt);
            if (desiredTargetIdRes.has_value())
            {
                request.DesiredTarget = desiredTargetIdRes.value();
            }
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
    TP_ASSERT(pForm, "Form not found.");
    Actor* pActor = RTTI_CAST(pForm, TESForm, Actor);
    TP_ASSERT(pActor, "Form is not actor.");

    pActor->GenerateMagicCasters();

    // Only left hand casters need dual casting (?)
    pActor->leftHandCaster->SetDualCasting(acMessage.IsDualCasting);

    MagicItem* pSpell = nullptr;

    if (acMessage.CastingSource >= 4)
    {
        spdlog::warn("Casting source out of bounds, trying form id");
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
        else
            pSpell = RTTI_CAST(pSpellForm, TESForm, MagicItem);
    }
    else
    {
        pSpell = pActor->magicItems[acMessage.CastingSource];
    }

    if (!pSpell)
    {
        spdlog::error("Could not find spell.");
        return;
    }

    TESForm* pDesiredTargetForm = nullptr;

    if (acMessage.DesiredTarget != 0)
    {
        auto view = m_world.view<FormIdComponent>();
        for (auto entity : view)
        {
            std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
            if (!serverIdRes.has_value())
                continue;

            uint32_t serverId = serverIdRes.value();
        
            if (serverId == acMessage.DesiredTarget)
            {
                const auto& formIdComponent = view.get<FormIdComponent>(entity);
                pDesiredTargetForm = TESForm::GetById(formIdComponent.Id);
            }
        }

    }

    TESObjectREFR* pDesiredTarget = RTTI_CAST(pDesiredTargetForm, TESForm, TESObjectREFR);

    switch (acMessage.CastingSource)
    {
    case MagicSystem::CastingSource::LEFT_HAND:
        pActor->leftHandCaster->CastSpellImmediate(pSpell, false, pDesiredTarget, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::RIGHT_HAND:
        pActor->rightHandCaster->CastSpellImmediate(pSpell, false, pDesiredTarget, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::OTHER:
        pActor->shoutCaster->CastSpellImmediate(pSpell, false, pDesiredTarget, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::INSTANT:
        pActor->instantCaster->CastSpellImmediate(pSpell, false, pDesiredTarget, 1.0f, false, 0.0f);
        break;
    }
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
        return;

    auto& localComponent = view.get<LocalComponent>(*casterEntityIt);

    InterruptCastRequest request;
    request.CasterId = localComponent.Id;
    m_transport.Send(request);
#endif
}

void MagicService::OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept
{
#if TP_SKYRIM64
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
    Actor* pActor = RTTI_CAST(pForm, TESForm, Actor);

    pActor->InterruptCast(false);

    spdlog::debug("Interrupt remote cast successful");
#endif
}

void MagicService::OnAddTargetEvent(const AddTargetEvent& acEvent) noexcept
{
#if TP_SKYRIM64
    if (!m_transport.IsConnected())
        return;

    AddTargetRequest request;

    if (!m_world.GetModSystem().GetServerModId(acEvent.SpellID, request.SpellId.ModId, request.SpellId.BaseId))
    {
        spdlog::error("{s}: Could not find spell with form {:X}", __FUNCTION__, acEvent.SpellID);
        return;
    }

    if (!m_world.GetModSystem().GetServerModId(acEvent.EffectID, request.EffectId.ModId, request.EffectId.BaseId))
    {
        spdlog::error("{s}: Could not find effect with form {:X}", __FUNCTION__, acEvent.EffectID);
        return;
    }

    request.Magnitude = acEvent.Magnitude;

    auto view = m_world.view<FormIdComponent>();
    const auto it = std::find_if(std::begin(view), std::end(view), [id = acEvent.TargetID, view](auto entity) {
        return view.get<FormIdComponent>(entity).Id == id;
    });

    if (it == std::end(view))
    {
        spdlog::warn("Target not found for magic add target, form id: {:X}", acEvent.TargetID);
        m_queuedEffects[acEvent.TargetID] = request;
        return;
    }

    entt::entity entity = *it;

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
    if (!serverIdRes.has_value())
        return;

    request.TargetId = serverIdRes.value();
    m_transport.Send(request);
#endif
}

void MagicService::OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept
{
#if TP_SKYRIM64
    std::optional<Actor*> pActorRes = Utils::GetActorByServerId(acMessage.TargetId);
    if (!pActorRes.has_value())
        return;

    Actor* pActor = pActorRes.value();

    const uint32_t cSpellId = World::Get().GetModSystem().GetGameId(acMessage.SpellId);
    if (cSpellId == 0)
    {
        spdlog::error("{}: failed to retrieve spell id, GameId base: {:X}, mod: {:X}", __FUNCTION__,
                      acMessage.SpellId.BaseId, acMessage.SpellId.ModId);
        return;
    }

    MagicItem* pSpell = RTTI_CAST(TESForm::GetById(cSpellId), TESForm, MagicItem);
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
    data.fUnkFloat1 = 1.0f;
    data.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;

    pActor->magicTarget.AddTarget(data);
#endif
}

BSTEventResult MagicService::OnEvent(const TESMagicEffectApplyEvent* apEvent, const EventDispatcher<TESMagicEffectApplyEvent>*)
{
#if MAGIC_DEBUG
    spdlog::warn("TESMagicEffectApplyEvent, target: {:X}, caster: {:X}, effect id: {:X}",
                 apEvent->hTarget ? apEvent->hTarget->formID : 0,
                 apEvent->hCaster ? apEvent->hCaster->formID : 0,
                 apEvent->uiMagicEffectFormID);
#endif

    return BSTEventResult::kOk;
}

BSTEventResult MagicService::OnEvent(const TESActiveEffectApplyRemove* apEvent, const EventDispatcher<TESActiveEffectApplyRemove>*)
{
#if MAGIC_DEBUG
    spdlog::error("TESActiveEffectApplyRemove, target: {:X}, caster: {:X}, effect id: {:X}, applied? {}",
                 apEvent->hTarget ? apEvent->hTarget->formID : 0,
                 apEvent->hCaster ? apEvent->hCaster->formID : 0,
                 apEvent->usActiveEffectUniqueID,
                 apEvent->bIsApplied);
#endif

    return BSTEventResult::kOk;
}
