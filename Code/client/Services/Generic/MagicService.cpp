#include <Services/MagicService.h>

#include <World.h>

#include <Events/UpdateEvent.h>
#include <Events/SpellCastEvent.h>
#include <Events/InterruptCastEvent.h>
#include <Events/AddTargetEvent.h>

#include <Messages/SpellCastRequest.h>
#include <Messages/InterruptCastRequest.h>
#include <Messages/AddTargetRequest.h>

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

    auto* pEventList = EventDispatcherManager::Get();
    pEventList->magicEffectApplyEvent.RegisterSink(this);
    pEventList->activeEffectApplyRemove.RegisterSink(this);
}

// TODO: might be easier to just check the list of effects in OnUpdate() for each actor, and send that back and forth

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

    for (auto [formId, spellId] : m_queuedEffects)
    {
        auto view = m_world.view<FormIdComponent>();
        const auto it = std::find_if(std::begin(view), std::end(view), [id = formId, view](auto entity) {
            return view.get<FormIdComponent>(entity).Id == id;
        });

        if (it == std::end(view))
            continue;

        entt::entity entity = *it;

        AddTargetRequest request;

        std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
        if (!serverIdRes.has_value())
            continue;

        request.TargetId = serverIdRes.value();

        if (!m_world.GetModSystem().GetServerModId(spellId, request.SpellId.ModId, request.SpellId.BaseId))
        {
            spdlog::error("{s}: Could not find spell with form {:X}", __FUNCTION__, spellId);
            continue;
        }

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
    if (auto* pSpell = RTTI_CAST(acSpellCastEvent.pSpell, MagicItem, SpellItem))
    {
        if (pSpell->eCastingType != MagicSystem::CastingType::CONCENTRATION)
        {
            spdlog::warn("Canceled magic spell");
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
                spdlog::info("has_value");
                request.DesiredTarget = desiredTargetIdRes.value();
            }
        }
    }

    spdlog::info("Spell cast event sent, ID: {:X}, Source: {}, IsDualCasting: {}, desired target: {:X}", request.CasterId,
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
            spdlog::error("Cannot find spell form.");
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

    TESForm* pDesiredTarget = nullptr;

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
                pDesiredTarget = TESForm::GetById(formIdComponent.Id);
                if (pDesiredTarget)
                    spdlog::info("Desired target: {:X}", pDesiredTarget->formID);
            }
        }

    }

    switch (acMessage.CastingSource)
    {
    case MagicSystem::CastingSource::LEFT_HAND:
        pActor->leftHandCaster->CastSpellImmediate(pSpell, false, (TESObjectREFR*)pDesiredTarget, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::RIGHT_HAND:
        pActor->rightHandCaster->CastSpellImmediate(pSpell, false, (TESObjectREFR*)pDesiredTarget, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::OTHER:
        pActor->shoutCaster->CastSpellImmediate(pSpell, false, (TESObjectREFR*)pDesiredTarget, 1.0f, false, 0.0f);
        break;
    case MagicSystem::CastingSource::INSTANT:
        // TODO: instant?
        break;
    }
#endif
}

void MagicService::OnInterruptCastEvent(const InterruptCastEvent& acEvent) const noexcept
{
#if TP_SKYRIM64
    if (!m_transport.IsConnected())
        return;

    auto formId = acEvent.CasterFormID;

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

    auto view = m_world.view<FormIdComponent>();
    const auto it = std::find_if(std::begin(view), std::end(view), [id = acEvent.TargetID, view](auto entity) {
        return view.get<FormIdComponent>(entity).Id == id;
    });

    if (it == std::end(view))
    {
        spdlog::warn("Target not found for magic add target, form id: {:X}", acEvent.TargetID);
        m_queuedEffects[acEvent.TargetID] = acEvent.SpellID;
        return;
    }

    entt::entity entity = *it;

    AddTargetRequest request;

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
    if (!serverIdRes.has_value())
        return;

    request.TargetId = serverIdRes.value();

    if (!m_world.GetModSystem().GetServerModId(acEvent.SpellID, request.SpellId.ModId, request.SpellId.BaseId))
    {
        spdlog::error("{s}: Could not find spell with form {:X}", __FUNCTION__, acEvent.SpellID);
        return;
    }

    m_transport.Send(request);
#endif
}

void MagicService::OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept
{
#if TP_SKYRIM64
    auto view = m_world.view<FormIdComponent>();

    for (auto entity : view)
    {
        std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
        if (!serverIdRes.has_value())
            continue;

        uint32_t serverId = serverIdRes.value();

        if (serverId == acMessage.TargetId)
        {
            auto& formIdComponent = view.get<FormIdComponent>(entity);
            auto* pForm = TESForm::GetById(formIdComponent.Id);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

            TP_ASSERT(pActor, "Actor should exist, form id: {:X}", formIdComponent.Id);

            if (pActor)
            {
                const auto cSpellId = World::Get().GetModSystem().GetGameId(acMessage.SpellId);
                if (cSpellId == 0)
                {
                    spdlog::error("{}: failed to retrieve spell id, GameId base: {:X}, mod: {:X}", __FUNCTION__,
                                  acMessage.SpellId.BaseId, acMessage.SpellId.ModId);
                    return;
                }

                auto* pSpell = static_cast<MagicItem*>(TESForm::GetById(cSpellId));
                if (!pSpell)
                {
                    spdlog::error("{}: Failed to retrieve spell by id {:X}", cSpellId);
                    return;
                }

                // TODO: AddTarget gets notified for every effect, but we loop through the effects here again
                for (auto effect : pSpell->listOfEffects)
                {
                    MagicTarget::AddTargetData data{};
                    data.pSpell = pSpell;
                    data.pEffectItem = effect;
                    data.fMagnitude = 0.0f;
                    data.fUnkFloat1 = 1.0f;
                    data.eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;

                    pActor->magicTarget.AddTarget(data);
                }

                break;
            }
        }
    }
#endif
}

BSTEventResult MagicService::OnEvent(const TESMagicEffectApplyEvent* apEvent, const EventDispatcher<TESMagicEffectApplyEvent>*)
{
    spdlog::warn("TESMagicEffectApplyEvent, target: {:X}, caster: {:X}, effect id: {:X}",
                 apEvent->hTarget ? apEvent->hTarget->formID : 0,
                 apEvent->hCaster ? apEvent->hCaster->formID : 0,
                 apEvent->uiMagicEffectFormID);

    return BSTEventResult::kOk;
}

BSTEventResult MagicService::OnEvent(const TESActiveEffectApplyRemove* apEvent, const EventDispatcher<TESActiveEffectApplyRemove>*)
{
    spdlog::error("TESActiveEffectApplyRemove, target: {:X}, caster: {:X}, effect id: {:X}, applied? {}",
                 apEvent->hTarget ? apEvent->hTarget->formID : 0,
                 apEvent->hCaster ? apEvent->hCaster->formID : 0,
                 apEvent->usActiveEffectUniqueID,
                 apEvent->bIsApplied);

    return BSTEventResult::kOk;
}
