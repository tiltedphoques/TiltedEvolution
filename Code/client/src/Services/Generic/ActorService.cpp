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
#include <Events/HealthChangeEvent.h>

#include <Messages/NotifyActorValueChanges.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/NotifyActorMaxValueChanges.h>
#include <Messages/RequestActorMaxValueChanges.h>
#include <Messages/NotifyHealthChangeBroadcast.h>
#include <Messages/RequestHealthChangeBroadcast.h>

#include <misc/ActorValueOwner.h>

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
    aDispatcher.sink<NotifyActorMaxValueChanges>().connect<&ActorService::OnActorMaxValueChanges>(this);
    aDispatcher.sink<HealthChangeEvent>().connect<&ActorService::OnHealthChange>(this);
    aDispatcher.sink<NotifyHealthChangeBroadcast>().connect<&ActorService::OnHealthChangeBroadcast>(this);
}

ActorService::~ActorService() noexcept
{
}

void ActorService::AddToActorMap(uint32_t aId, Actor* aActor) noexcept
{
    Map<uint32_t, float> values;
    Map<uint32_t, float> maxValues;

    int amountOfValues;
#if TP_SKYRIM64
    amountOfValues = 164;
#elif TP_FALLOUT4
    amountOfValues = 132;
#endif

    for (int i = 0; i < amountOfValues; i++)
    {
#if TP_FALLOUT4
        if (i == 23 || i == 48 || i == 70)
            continue;
#endif
        float value = GetActorValue(aActor, i);
        values.insert({i, value});
        float maxValue = GetActorMaxValue(aActor, i);
        maxValues.insert({i, maxValue});
    }

    m_actorMaxValues.insert({aId, maxValues});
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
    RunSmallHealthUpdates();
    m_timeSinceDiff += acEvent.Delta;
    if (m_timeSinceDiff >= 1)
    {
        m_timeSinceDiff = 0;

        BroadcastAllActorValues();
    }
}

void ActorService::BroadcastAllActorValues() noexcept
{
    BroadcastActorValues(&m_actorValues, ValueType::kValue);
    BroadcastActorValues(&m_actorMaxValues, ValueType::kMaxValue);
}

void ActorService::BroadcastActorValues(Map<uint32_t, Map<uint32_t, float>>* aActorValues, uint8_t aValueType) noexcept
{
    auto view = m_world.view<FormIdComponent, LocalComponent>();

    for (auto& value : *aActorValues)
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
                    if (aValueType == ValueType::kValue)
                    {
                        RequestActorValueChanges requestChanges;
                        requestChanges.m_Id = value.first;

                        int amountOfValues;
#if TP_SKYRIM64
                        amountOfValues = 164;
#elif TP_FALLOUT4
                        amountOfValues = 132;
#endif

                        for (int i = 0; i < amountOfValues; i++)
                        {
#if TP_FALLOUT4
                            if (i == 23 || i == 48 || i == 70)
                                continue;
#endif
                            float newValue = GetActorValue(pActor, i);
                            float oldValue = value.second[i];
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
                    else if (aValueType == ValueType::kMaxValue)
                    {
                        RequestActorMaxValueChanges requestChanges;
                        requestChanges.m_Id = value.first;

                        int amountOfValues;
#if TP_SKYRIM64
                        amountOfValues = 164;
#elif TP_FALLOUT4
                        amountOfValues = 132;
#endif

                        for (int i = 0; i < amountOfValues; i++)
                        {
#if TP_FALLOUT4
                            if (i == 23 || i == 48 || i == 70)
                                continue;
#endif
                            float newValue = GetActorMaxValue(pActor, i);
                            float oldValue = value.second[i];
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

void ActorService::OnHealthChange(const HealthChangeEvent& acEvent) noexcept
{
    if (m_transport.IsConnected())
    {
        auto view = m_world.view<FormIdComponent>();

        for (auto entity : view)
        {
            auto& formIdComponent = view.get(entity);
            if (formIdComponent.Id == acEvent.Hittee->formID)
            {
                const auto localComponent = m_world.try_get<LocalComponent>(entity);
                const auto remoteComponent = m_world.try_get<RemoteComponent>(entity);

                if (localComponent)
                {
                    if (acEvent.DeltaHealth > -1.0f && acEvent.DeltaHealth < 1.0f)
                    {
                        if (m_smallHealthChanges.find(localComponent->Id) == m_smallHealthChanges.end())
                            m_smallHealthChanges[localComponent->Id] = acEvent.DeltaHealth;
                        else
                            m_smallHealthChanges[localComponent->Id] += acEvent.DeltaHealth;
                        return;
                    }

                    RequestHealthChangeBroadcast requestHealthChange;
                    requestHealthChange.m_Id = localComponent->Id;
                    requestHealthChange.m_DeltaHealth = acEvent.DeltaHealth;

                    m_transport.Send(requestHealthChange);

                    spdlog::info("Sent out delta health through collection: {:x}:{:f}", localComponent->Id, acEvent.DeltaHealth);
                }
                else if (remoteComponent)
                {
                    if (-1.0f < acEvent.DeltaHealth < 1.0f)
                    {
                        if (m_smallHealthChanges.find(remoteComponent->Id) == m_smallHealthChanges.end())
                            m_smallHealthChanges[remoteComponent->Id] = acEvent.DeltaHealth;
                        else
                            m_smallHealthChanges[remoteComponent->Id] += acEvent.DeltaHealth;
                        return;
                    }

                    RequestHealthChangeBroadcast requestHealthChange;
                    requestHealthChange.m_Id = remoteComponent->Id;
                    requestHealthChange.m_DeltaHealth = acEvent.DeltaHealth;

                    m_transport.Send(requestHealthChange);

                    spdlog::info("Sent out delta health through collection: {:x}:{:f}", remoteComponent->Id, acEvent.DeltaHealth);
                }
            }
        }
    }
}

void ActorService::RunSmallHealthUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 250ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    if (!m_smallHealthChanges.empty())
    {
        for (auto& value : m_smallHealthChanges)
        {
            RequestHealthChangeBroadcast requestHealthChange;
            requestHealthChange.m_Id = value.first;
            requestHealthChange.m_DeltaHealth = value.second;

            m_transport.Send(requestHealthChange);

            spdlog::info("Sent out delta health through timer. {:x}:{:f}", value.first, value.second);
        }

        m_smallHealthChanges.clear();
    }
}

void ActorService::OnHealthChangeBroadcast(const NotifyHealthChangeBroadcast& acEvent) noexcept
{
    auto view = m_world.view<FormIdComponent>();

    for (auto entity : view)
    {
        uint32_t componentId;
        const auto localComponent = m_world.try_get<LocalComponent>(entity);
        const auto remoteComponent = m_world.try_get<RemoteComponent>(entity);
        if (localComponent)
            componentId = localComponent->Id;
        else if (remoteComponent)
            componentId = remoteComponent->Id;
        else
            continue;

        if (componentId == acEvent.m_Id)
        {
            auto& formIdComponent = view.get(entity);
            auto* pForm = TESForm::GetById(formIdComponent.Id);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

            if (pActor != NULL)
            {
                float newHealth = GetActorValue(pActor, ActorValueInfo::kHealth) + acEvent.m_DeltaHealth;
                ForceActorValue(pActor, 2, ActorValueInfo::kHealth, newHealth);
            }
        }
    }
}

void ActorService::OnActorValueChanges(const NotifyActorValueChanges& acEvent) noexcept
{
    auto view = m_world.view<FormIdComponent, RemoteComponent>();

    const auto itor = std::find_if(std::begin(view), std::end(view), [id = acEvent.m_Id, view](entt::entity entity) 
    {
        return view.get<RemoteComponent>(entity).Id == id;
    });

    if (itor == std::end(view))
        return;

    auto& formIdComponent = view.get<FormIdComponent>(*itor);
    auto& remoteComponent = view.get<RemoteComponent>(*itor);
    auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);

    if (!pActor)
        return;

    for (auto& value : acEvent.m_values)
    {
        std::cout << "Form ID: " << std::hex << formIdComponent.Id << " Remote ID: " << std::hex << acEvent.m_Id << std::endl;
        std::cout << "Key: " << std::dec << value.first << " Value: " << value.second << std::endl;

        if (value.first == ActorValueInfo::kHealth)
            continue;

#if TP_SKYRIM64
        if (value.first == ActorValueInfo::kStamina || value.first == ActorValueInfo::kMagicka)
        {
            ForceActorValue(pActor, 2, value.first, value.second);
        }
#endif
        else
        {
            SetActorValue(pActor, value.first, value.second);
        }
    }

    /*
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

                    if (value.first == ActorValueInfo::kHealth)
                        continue;
#if TP_SKYRIM64
                    if (value.first == ActorValueInfo::kStamina || value.first == ActorValueInfo::kMagicka)
                    {
                        ForceActorValue(pActor, 2, value.first, value.second);
                    }
#endif
                    else
                    {
                        SetActorValue(pActor, value.first, value.second);
                    }
                }
            }
        }
    }
    */
}

void ActorService::OnActorMaxValueChanges(const NotifyActorMaxValueChanges& acEvent) noexcept
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
                    std::cout << "Max values update." << std::endl;
                    std::cout << "Form ID: " << std::hex << formIdComponent.Id << " Remote ID: " << std::hex << acEvent.m_Id << std::endl;
                    std::cout << "Key: " << std::dec << value.first << " Value: " << value.second << std::endl;

                    ForceActorValue(pActor, 0, value.first, value.second);
                }
            }
        }
    }
}

void ActorService::ForceActorValue(Actor* apActor, uint32_t aMode, uint32_t aId, float aValue) noexcept
{
    float current = GetActorValue(apActor, aId);
#if TP_FALLOUT4
    ActorValueInfo* pActorValueInfo = apActor->GetActorValueInfo(aId);
    apActor->actorValueOwner.ForceCurrent(aMode, pActorValueInfo, aValue - current);
#elif TP_SKYRIM64
    apActor->actorValueOwner.ForceCurrent(aMode, aId, aValue - current);
#endif
}

void ActorService::SetActorValue(Actor* apActor, uint32_t aId, float aValue) noexcept
{
#if TP_FALLOUT4
    ActorValueInfo* pActorValueInfo = apActor->GetActorValueInfo(aId);
    apActor->actorValueOwner.SetValue(pActorValueInfo, aValue);
#elif TP_SKYRIM64
    apActor->actorValueOwner.SetValue(aId, aValue);
#endif
}

float ActorService::GetActorValue(Actor* apActor, uint32_t aId) noexcept
{
#if TP_FALLOUT4
    ActorValueInfo* pActorValueInfo = apActor->GetActorValueInfo(aId);
    return apActor->actorValueOwner.GetValue(pActorValueInfo);
#elif TP_SKYRIM64
    return apActor->actorValueOwner.GetValue(aId);
#endif
}

float ActorService::GetActorMaxValue(Actor* apActor, uint32_t aId) noexcept
{
#if TP_FALLOUT4
    ActorValueInfo* pActorValueInfo = apActor->GetActorValueInfo(aId);
    return apActor->actorValueOwner.GetMaxValue(pActorValueInfo);
#elif TP_SKYRIM64
    return apActor->actorValueOwner.GetMaxValue(aId);
#endif
}
