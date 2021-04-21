#include <TiltedOnlinePCH.h>


#include <World.h>
#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/ActivateEvent.h>
#include <Services/EnvironmentService.h>
#include <Services/ImguiService.h>
#include <Messages/ServerTimeSettings.h>
#include <Messages/ActivateRequest.h>
#include <Messages/NotifyActivate.h>

#include <TimeManager.h>
#include <PlayerCharacter.h>

#include <imgui.h>
#include <inttypes.h>

#define ENVIRONMENT_DEBUG 0

constexpr float kTransitionSpeed = 5.f;

bool EnvironmentService::s_gameClockLocked = false;

bool EnvironmentService::AllowGameTick() noexcept
{
    return !s_gameClockLocked;
}

EnvironmentService::EnvironmentService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService, TransportService& aTransport) 
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_timeUpdateConnection = aDispatcher.sink<ServerTimeSettings>().connect<&EnvironmentService::OnTimeUpdate>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::HandleUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&EnvironmentService::OnDisconnected>(this);
    m_onActivateConnection = aDispatcher.sink<ActivateEvent>().connect<&EnvironmentService::OnActivate>(this);
    m_activateConnection = aDispatcher.sink<NotifyActivate>().connect<&EnvironmentService::OnActivateNotify>(this);

#if ENVIRONMENT_DEBUG
    m_drawConnection = aImguiService.OnDraw.connect<&EnvironmentService::OnDraw>(this);
#endif

#if TP_SKYRIM64
    EventDispatcherManager::Get()->activateEvent.RegisterSink(this);
#else
    GetEventDispatcher_TESActivateEvent()->RegisterSink(this);
#endif
}

void EnvironmentService::OnTimeUpdate(const ServerTimeSettings& acMessage) noexcept
{
    // disable the game clock
    m_onlineTime.TimeScale = acMessage.TimeScale;
    m_onlineTime.Time = acMessage.Time;
    ToggleGameClock(false);
}

void EnvironmentService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // signal a time transition
    m_fadeTimer = 0.f;
    m_switchToOffline = true;
}

BSTEventResult EnvironmentService::OnEvent(const TESActivateEvent* acEvent, const EventDispatcher<TESActivateEvent>* dispatcher)
{
#if ENVIRONMENT_DEBUG
    auto view = m_world.view<InteractiveObjectComponent>();

    const auto itor =
        std::find_if(std::begin(view), std::end(view), [id = acEvent->object->formID, view](entt::entity entity) {
            return view.get<InteractiveObjectComponent>(entity).Id == id;
        });

    if (itor == std::end(view))
    {
        AddObjectComponent(acEvent->object);
    }
#endif

    return BSTEventResult::kOk;
}

void EnvironmentService::AddObjectComponent(TESObjectREFR* apObject) noexcept
{
    auto entity = m_world.create();
    auto& interactiveObjectComponent = m_world.emplace<InteractiveObjectComponent>(entity);
    interactiveObjectComponent.Id = apObject->formID;
}

void EnvironmentService::OnActivate(const ActivateEvent& acEvent) noexcept
{
    if (acEvent.ActivateFlag)
        acEvent.pObject->Activate(acEvent.pActivator, acEvent.unk1, acEvent.unk2, acEvent.unk3, acEvent.unk4);

    if (!m_transport.IsConnected())
        return;

    uint32_t baseId = 0;
    uint32_t modId = 0;
    if (!m_world.GetModSystem().GetServerModId(acEvent.pObject->formID, modId, baseId))
        return;

    uint32_t cellBaseId = 0;
    uint32_t cellModId = 0;
    if (!m_world.GetModSystem().GetServerModId(acEvent.pObject->GetCellId(), cellModId, cellBaseId))
        return;

    ActivateRequest request;
    request.Id.BaseId = baseId;
    request.Id.ModId = modId;
    request.CellId.BaseId = cellBaseId;
    request.CellId.ModId = cellModId;

    auto view = m_world.view<FormIdComponent>();
    const auto pEntity =
        std::find_if(std::begin(view), std::end(view), [id = acEvent.pActivator->formID, view](entt::entity entity) {
            return view.get<FormIdComponent>(entity).Id == id;
        });

    // Only sync activations triggered by actors
    if (pEntity == std::end(view))
        return;

    const auto localComponent = m_world.try_get<LocalComponent>(*pEntity);
    const auto remoteComponent = m_world.try_get<RemoteComponent>(*pEntity);
    request.ActivatorId = localComponent ? localComponent->Id : remoteComponent->Id;

    m_transport.Send(request);
}

void EnvironmentService::OnActivateNotify(const NotifyActivate& acMessage) noexcept
{
    auto view = m_world.view<FormIdComponent>();
    for (auto entity : view)
    {
        uint32_t componentId;
        const auto cpLocalComponent = m_world.try_get<LocalComponent>(entity);
        const auto cpRemoteComponent = m_world.try_get<RemoteComponent>(entity);

        if (cpLocalComponent)
            componentId = cpLocalComponent->Id;
        else if (cpRemoteComponent)
            componentId = cpRemoteComponent->Id;
        else
            continue;

        if (componentId == acMessage.ActivatorId)
        {
            const auto cObjectId = World::Get().GetModSystem().GetGameId(acMessage.Id);
            if (cObjectId == 0)
            {
                spdlog::error("Failed to retrieve object to activate.");
                return;
            }

            auto* pObject = RTTI_CAST(TESForm::GetById(cObjectId), TESForm, TESObjectREFR);
            if (!pObject)
            {
                spdlog::error("Failed to retrieve object to activate.");
                return;
            }

            auto& formIdComponent = view.get<FormIdComponent>(entity);
            auto* pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
            
            if (pActor)
            {
                // unsure if these flags are the best, but these are passed with the papyrus Activate fn
                // might be an idea to have the client send the flags through NotifyActivate
                pObject->Activate(pActor, 0, 0, 1, 0);
                return;
            }
        }
    }
}

float EnvironmentService::TimeInterpolate(const TimeModel& aFrom, TimeModel& aTo) const
{
    const auto t = aTo.Time - aFrom.Time;
    if (t < 0.f)
    {
        const auto v = t + 24.f;
        // interpolate on the time difference, not the time
        const auto x = TiltedPhoques::Lerp(0.f, v, m_fadeTimer / kTransitionSpeed) + aFrom.Time;

        return TiltedPhoques::Mod(x, 24.f);
    }
    
    return TiltedPhoques::Lerp(aFrom.Time, aTo.Time, m_fadeTimer / kTransitionSpeed);
}

void EnvironmentService::ToggleGameClock(bool aEnable)
{
    auto* pGameTime = TimeData::Get();
    if (aEnable)
    {
        pGameTime->GameDay->i = m_offlineTime.Day;
        pGameTime->GameMonth->i = m_offlineTime.Month;
        pGameTime->GameYear->i = m_offlineTime.Year;
        pGameTime->TimeScale->f = m_offlineTime.TimeScale;
        pGameTime->GameDaysPassed->f = (m_offlineTime.Time * (1.f / 24.f)) + m_offlineTime.Day;
        pGameTime->GameHour->f = m_offlineTime.Time;
        m_switchToOffline = false;
    }
    else
    {
        m_offlineTime.Day = pGameTime->GameDay->i;
        m_offlineTime.Month = pGameTime->GameMonth->i;
        m_offlineTime.Year = pGameTime->GameYear->i;
        m_offlineTime.Time = pGameTime->GameHour->f;
        m_offlineTime.TimeScale = pGameTime->TimeScale->f;
    }

    s_gameClockLocked = !aEnable;
}

void EnvironmentService::HandleUpdate(const UpdateEvent& aEvent) noexcept
{
    if (s_gameClockLocked)
    {
        const auto updateDelta = static_cast<float>(aEvent.Delta);
        auto* pGameTime = TimeData::Get();

        if (!m_lastTick)
            m_lastTick = m_world.GetTick();

        const auto now = m_world.GetTick();

        if (m_switchToOffline)
        {
            // time transition out
            if (m_fadeTimer < kTransitionSpeed)
            {
                pGameTime->GameHour->f = TimeInterpolate(m_onlineTime, m_offlineTime);
                // before we quit here we fire this event
                if ((m_fadeTimer + updateDelta) > kTransitionSpeed)
                {
                    m_fadeTimer += updateDelta;
                    ToggleGameClock(true);
                }
                else
                    m_fadeTimer += updateDelta;
            }
        }

        // we got disconnected or the client got ahead of us
        if (now < m_lastTick)
            return;

        const auto delta = now - m_lastTick;
        m_lastTick = now;

        m_onlineTime.Update(delta);
        pGameTime->GameDay->i = m_onlineTime.Day;
        pGameTime->GameMonth->i = m_onlineTime.Month;
        pGameTime->GameYear->i = m_onlineTime.Year;
        pGameTime->TimeScale->f = m_onlineTime.TimeScale;
        pGameTime->GameDaysPassed->f = (m_onlineTime.Time * (1.f / 24.f)) + m_onlineTime.Day;

        // time transition in
        if (m_fadeTimer < kTransitionSpeed)
        {
            pGameTime->GameHour->f = TimeInterpolate(m_offlineTime, m_onlineTime);
            m_fadeTimer += updateDelta;
        }
        else
            pGameTime->GameHour->f = m_onlineTime.Time;
    }
}

void EnvironmentService::OnDraw() noexcept
{
#if ENVIRONMENT_DEBUG
    static uint32_t s_selectedFormId = 0;
    static uint32_t s_selected = 0;
    static uint32_t formId;

    ImGui::SetNextWindowSize(ImVec2(250, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin("Interactive object list");
    ImGui::BeginChild("Objects", ImVec2(0, 200), true);

    const auto view = m_world.view<InteractiveObjectComponent>();
    Vector<entt::entity> entities(view.begin(), view.end());

    int i = 0;
    for (auto entity : entities)
    {
        auto& objectComponent = view.get<InteractiveObjectComponent>(entity);
        auto* pForm = TESForm::GetById(objectComponent.Id);
        auto* pObject = RTTI_CAST(pForm, TESForm, TESObjectREFR);

        if (!pObject)
            continue;

        char name[256];
        sprintf_s(name, std::size(name), "%s (%x)", pObject->baseForm->GetName(), objectComponent.Id);
        if (ImGui::Selectable(name, s_selectedFormId == objectComponent.Id))
            s_selectedFormId = objectComponent.Id;

        if (s_selectedFormId == objectComponent.Id)
            s_selected = i;

        ++i;
    }

    ImGui::EndChild();

    if (s_selected < entities.size())
    {
        auto& objectComponent = view.get<InteractiveObjectComponent>(entities[s_selected]);
        auto* pForm = TESForm::GetById(objectComponent.Id);
        auto* pObject = RTTI_CAST(pForm, TESForm, TESObjectREFR);

        if (pObject)
        {
            uint64_t address = reinterpret_cast<uint64_t>(pObject);
            ImGui::InputScalar("Memory address", ImGuiDataType_U64, &address, 0, 0, "%" PRIx64, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
            if (ImGui::Button("Activate"))
            {
                auto* pActor = PlayerCharacter::Get();
                World::Get().GetRunner().Trigger(ActivateEvent(pObject, pActor, 0, 0, 1, 0, true));
            }
            int formType = pObject->GetFormType();
            ImGui::InputInt("Form type", &formType, 0, 0, ImGuiInputTextFlags_ReadOnly);
            int formTypeBase = pObject->baseForm->GetFormType();
            ImGui::InputInt("Form type base", &formTypeBase, 0, 0, ImGuiInputTextFlags_ReadOnly);
        }
    }

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &formId, 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);
    if (ImGui::Button("Get address from form ID"))
    {
        if (formId)
        {
            auto* pForm = TESForm::GetById(formId);
            auto* pObject = RTTI_CAST(pForm, TESForm, TESObjectREFR);
            if (pObject)
            {
                auto view = m_world.view<InteractiveObjectComponent>();

                const auto itor =
                    std::find_if(std::begin(view), std::end(view), [id = pObject->formID, view](entt::entity entity) {
                        return view.get<InteractiveObjectComponent>(entity).Id == id;
                    });

                if (itor == std::end(view))
                {
                    AddObjectComponent(pObject);
                }
            }
        }
    }

    ImGui::End();
#endif
}
