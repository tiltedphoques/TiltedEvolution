#include <Services/ActorService.h>
#include <World.h>
#include <Forms/ActorValueInfo.h>
#include <Games/References.h>
#include <Components.h>
#include <Events/UpdateEvent.h>
#include <Messages/NotifyActorValueChanges.h>
#include <Messages/RequestActorValueChanges.h>

ActorService::ActorService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_transport(aTransport)
{
    aDispatcher.sink<NotifyActorValueChanges>().connect<&ActorService::OnActorValueChanges>(this);
    EventDispatcherManager::Get()->hitEvent.RegisterSink(this);
}

ActorService::~ActorService() noexcept
{
}

BSTEventResult ActorService::OnEvent(const TESHitEvent* hitEvent, const EventDispatcher<TESHitEvent>* dispatcher)
{
    std::cout << "Damaged!" << std::endl;

    auto view = m_world.view<FormIdComponent, LocalComponent>();
    for (auto entity : view)
    {
        auto& formIdComponent = view.get<FormIdComponent>(entity);
        auto* pForm = TESForm::GetById(formIdComponent.Id);
        auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

        if (pActor != NULL)
        {
            RequestActorValueChanges requestChanges;
            requestChanges.m_formId = formIdComponent.Id;
            requestChanges.m_health = pActor->actorValueOwner.GetValue(ActorValueInfo::kHealth);
            m_transport.Send(requestChanges);
        }
    }

    return BSTEventResult::kAbort;
}

void ActorService::OnActorValueChanges(const NotifyActorValueChanges& acEvent) const noexcept
{
    auto* pForm = TESForm::GetById(acEvent.m_formId);
    auto* pActor = RTTI_CAST(pForm, TESForm, Actor);

    #if TP_SKYRIM
    if (pActor != NULL)
    {
        pActor->actorValueOwner.SetValue(24, acEvent.m_health);
    }
    #endif
}
