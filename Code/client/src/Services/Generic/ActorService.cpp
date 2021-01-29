#include <Services/ActorService.h>
#include <World.h>
#include <Forms/ActorValueInfo.h>
#include <Games/References.h>
#include <Components.h>

#include <Events/UpdateEvent.h>
#include <Events/ReferenceSpawnedEvent.h>
#include <Events/ReferenceRemovedEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/HitEvent.h>

#include <Messages/NotifyActorValueChanges.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/NotifyDamageEvent.h>
#include <Messages/RequestDamageEvent.h>

ActorService::ActorService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_world.on_construct<LocalComponent>().connect<&ActorService::OnLocalComponentAdded>(this);
    aDispatcher.sink<DisconnectedEvent>().connect<&ActorService::OnDisconnected>(this);
    aDispatcher.sink<ReferenceSpawnedEvent>().connect<&ActorService::OnReferenceSpawned>(this);
    aDispatcher.sink<ReferenceRemovedEvent>().connect<&ActorService::OnReferenceRemoved>(this);
    aDispatcher.sink<UpdateEvent>().connect<&ActorService::OnUpdate>(this);
    aDispatcher.sink<NotifyActorValueChanges>().connect<&ActorService::OnActorValueChanges>(this);
    aDispatcher.sink<NotifyDamageEvent>().connect<&ActorService::OnDamageEvent>(this);
    aDispatcher.sink<HitEvent>().connect<&ActorService::OnHit>(this);
}

ActorService::~ActorService() noexcept
{
}

void ActorService::AddToActorMap(uint32_t aId, Actor* aActor) noexcept
{
    Map<uint32_t, float> values;
    for (int i = 0; i < 164; i++)
    {
        float value = aActor->actorValueOwner.GetValue(i);
        values.insert({i, value});
    }
    m_actorValues.insert({aId, values});
}

void ActorService::OnLocalComponentAdded(entt::registry& aRegistry, const entt::entity aEntity) noexcept
{
    auto& formIdComponent = aRegistry.get<FormIdComponent>(aEntity);
    auto& localComponent = aRegistry.get<LocalComponent>(aEntity);
    auto* pForm = TESForm::GetById(formIdComponent.Id);
    auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

    if (pActor != NULL)
    {
        //std::cout << "Form: " << std::hex << formIdComponent.Id << " Local: " << std::hex << localComponent.Id << std::endl;
        AddToActorMap(localComponent.Id, pActor);
    }
}

void ActorService::OnDisconnected(const DisconnectedEvent& acEvent) noexcept
{
    m_actorValues.clear();
}

void ActorService::OnReferenceSpawned(const ReferenceSpawnedEvent& acEvent) noexcept
{
    if (m_transport.IsConnected())
    {
        auto* localComponent = m_world.try_get<LocalComponent>(acEvent.Entity);
        if (localComponent != NULL)
        {
            auto* pForm = TESForm::GetById(acEvent.FormId);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

            if (pActor != NULL)
            {
                AddToActorMap(localComponent->Id, pActor);
            }
        }
    }    
}

void ActorService::OnReferenceRemoved(const ReferenceRemovedEvent& acEvent) noexcept
{
    if (m_transport.IsConnected())
    {
        m_actorValues.erase(acEvent.FormId);
    }    
}

void ActorService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    m_timeSinceDiff += acEvent.Delta;
    if (m_timeSinceDiff >= 1)
    {
        m_timeSinceDiff = 0;

        auto view = m_world.view<FormIdComponent, LocalComponent>();

        for (auto& value : m_actorValues)
        {
            for (auto entity : view)
            {
                auto& localComponent = view.get<LocalComponent>(entity);
                if (localComponent.Id == value.first)
                {
                    auto& formIdComponent = view.get<FormIdComponent>(entity);
                    auto* pForm = TESForm::GetById(formIdComponent.Id);
                    auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

                    if (pActor != NULL)
                    {
                        RequestActorValueChanges requestChanges;
                        requestChanges.m_Id = value.first;

                        for (int i = 0; i < 164; i++)
                        {
                            float oldValue = value.second[i];
                            float newValue = pActor->actorValueOwner.GetValue(i);
                            if (newValue != oldValue)
                            {
                                requestChanges.m_values.insert({i, newValue});
                                value.second[i] = newValue;
                            }
                        }

                        if (requestChanges.m_values.size() > 0)
                        {
                            m_transport.Send(requestChanges);
                        }
                    }
                }
            }
        }        
    }    
}

void ActorService::OnHit(const HitEvent& acEvent) noexcept
{
    auto view = m_world.view<FormIdComponent>();

    for (auto entity : view)
    {
        auto& formIdComponent = view.get(entity);
        if (formIdComponent.Id == acEvent.Hittee->formID)
        {
            const auto localComponent = m_world.try_get<LocalComponent>(entity);

            if (localComponent)
            {
                RequestDamageEvent requestDamageEvent;
                requestDamageEvent.m_Id = localComponent->Id;
                requestDamageEvent.m_Damage = acEvent.Damage;

                m_transport.Send(requestDamageEvent);
            }
            else
            {
                const auto remoteComponent = m_world.try_get<RemoteComponent>(entity);

                RequestDamageEvent requestDamageEvent;
                requestDamageEvent.m_Id = remoteComponent->Id;
                requestDamageEvent.m_Damage = acEvent.Damage;

                m_transport.Send(requestDamageEvent);
            }
        }
    }
}

void ActorService::OnDamageEvent(const NotifyDamageEvent& acEvent) noexcept
{
    auto view = m_world.view<FormIdComponent>();

    for (auto entity : view)
    {
        uint32_t componentId;
        const auto localComponent = m_world.try_get<LocalComponent>(entity);
        const auto remoteComponent = m_world.try_get<RemoteComponent>(entity);
        if (localComponent)
            componentId = localComponent->Id;
        else
            componentId = remoteComponent->Id;

        if (componentId == acEvent.m_Id)
        {
            auto& formIdComponent = view.get(entity);
            auto* pForm = TESForm::GetById(formIdComponent.Id);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

            if (pActor != NULL)
            {
                float newHealth = pActor->actorValueOwner.GetValue(ActorValueInfo::kHealth) - acEvent.m_Damage;
                ForceActorValue(pActor, ActorValueInfo::kHealth, newHealth);
            }
        }
    }
}

void ActorService::OnActorValueChanges(const NotifyActorValueChanges& acEvent) noexcept
{
    auto view = m_world.view<FormIdComponent, RemoteComponent>();

    for (auto entity : view)
    {
        auto& remoteComponent = view.get<RemoteComponent>(entity);
        if (remoteComponent.Id == acEvent.m_Id)
        {
            auto& formIdComponent = view.get<FormIdComponent>(entity);
            auto* pForm = TESForm::GetById(formIdComponent.Id);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

            if (pActor != NULL)
            {
                for (auto& value : acEvent.m_values)
                {
                    std::cout << "Form ID: " << std::hex << formIdComponent.Id << " Remote ID: " << std::hex << acEvent.m_Id << std::endl;
                    std::cout << "Key: " << std::dec << value.first << " Value: " << value.second << std::endl;

                    //if (value.first == ActorValueInfo::kHealth)
                        //continue;
                    if (value.first == ActorValueInfo::kHealth || value.first == ActorValueInfo::kStamina || value.first == ActorValueInfo::kMagicka)
                    {
                        ForceActorValue(pActor, value.first, value.second);
                    }
                    else
                    {
                        pActor->actorValueOwner.SetValue(value.first, value.second);
                    }                    
                }
            }
        }
    }
}

void ActorService::ForceActorValue(Actor* aActor, uint32_t aId, float aValue) noexcept
{
    float current = aActor->actorValueOwner.GetValue(aId);
    aActor->actorValueOwner.ForceCurrent(2, aId, aValue - current);
}
