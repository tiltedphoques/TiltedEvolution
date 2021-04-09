#include <TiltedOnlinePCH.h>


#include <World.h>
#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/ActivateEvent.h>
#include <Services/EnvironmentService.h>
#include <Services/ImguiService.h>
#include <Messages/ServerTimeSettings.h>

#include <TimeManager.h>
#include <PlayerCharacter.h>

#include <imgui.h>
#include <inttypes.h>

constexpr float kTransitionSpeed = 5.f;

bool EnvironmentService::s_gameClockLocked = false;

bool EnvironmentService::AllowGameTick() noexcept
{
    return !s_gameClockLocked;
}

EnvironmentService::EnvironmentService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService) : m_world(aWorld)
{
    m_timeUpdateConnection = aDispatcher.sink<ServerTimeSettings>().connect<&EnvironmentService::OnTimeUpdate>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::HandleUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&EnvironmentService::OnDisconnected>(this);

    m_drawConnection = aImguiService.OnDraw.connect<&EnvironmentService::OnDraw>(this);

    aDispatcher.sink<ActivateEvent>().connect<&EnvironmentService::OnActivate>(this);

#if TP_SKYRIM64
    EventDispatcherManager::Get()->activateEvent.RegisterSink(this);
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
    spdlog::info("Activated {:p}", (void*)acEvent->object);

    auto view = m_world.view<InteractiveObjectComponent>();

    const auto itor =
        std::find_if(std::begin(view), std::end(view), [id = acEvent->object->formID, view](entt::entity entity) {
            return view.get<InteractiveObjectComponent>(entity).Id == id;
        });

    if (itor == std::end(view))
    {
        AddObjectComponent(acEvent->object);
    }

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
    acEvent.pObject->Activate(acEvent.pActivator, acEvent.unk1, acEvent.unk2, acEvent.unk3, acEvent.unk4);
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
    static uint32_t s_selectedFormId = 0;
    static uint32_t s_selected = 0;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
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

        uint64_t address = reinterpret_cast<uint64_t>(pObject);
        ImGui::InputScalar("Memory address", ImGuiDataType_U64, &address, 0, 0, "%" PRIx64, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        if (ImGui::Button("Activate"))
        {
            auto* pActor = PlayerCharacter::Get();
            World::Get().GetRunner().Trigger(ActivateEvent(pObject, pActor, 0, 0, 1, 0));
        }
    }

    ImGui::End();
}
