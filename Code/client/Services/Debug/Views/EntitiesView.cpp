#include <Services/TestService.h>

#include <World.h>
#include <imgui.h>

void TestService::DrawEntitiesView()
{
    const auto view = m_world.view<FormIdComponent>();
    if (view.empty())
        return;

    ImGui::SetNextWindowSize(ImVec2(250, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin("Entities");

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Actors"))
        {
            DisplayEntities();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Objects"))
        {
            DisplayObjects();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

void TestService::DisplayEntities() noexcept
{
    static uint32_t s_selected = 0;

    StackAllocator<1 << 12> allocator;
    ScopedAllocator _{ allocator };

    const auto view = m_world.view<FormIdComponent>(entt::exclude<InteractiveObjectComponent>);

    Vector<entt::entity> entities(view.begin(), view.end());

    ImGui::Text("Actor list (%d)", entities.size());

    ImGui::BeginChild("Entities", ImVec2(0, 200), true);

    int i = 0;
    for(auto it : entities)
    {
        auto& formComponent = view.get<FormIdComponent>(it);
        const auto pActor = Cast<Actor>(TESForm::GetById(formComponent.Id));

        if (!pActor || !pActor->baseForm)
            continue;

        char name[256];
        sprintf_s(name, std::size(name), "%s (%x)", pActor->baseForm->GetName(), formComponent.Id);

        if (ImGui::Selectable(name, m_formId == formComponent.Id))
        {
            m_formId = formComponent.Id;
        }

        if(m_formId == formComponent.Id)
            s_selected = i;

        ++i;
    }

    ImGui::EndChild();

    if (s_selected < entities.size())
        DisplayEntityPanel(entities[s_selected]);
}

void TestService::DisplayObjects() noexcept
{
    static uint32_t s_selected = 0;

    StackAllocator<1 << 12> allocator;
    ScopedAllocator _{ allocator };

    const auto view = m_world.view<FormIdComponent, InteractiveObjectComponent>();

    Vector<entt::entity> entities(view.begin(), view.end());

    ImGui::Text("Object list (%d)", entities.size());

    ImGui::BeginChild("Entities", ImVec2(0, 200), true);

    int i = 0;
    for(auto it : entities)
    {
        auto& formComponent = view.get<FormIdComponent>(it);
        const auto pRefr = Cast<TESObjectREFR>(TESForm::GetById(formComponent.Id));

        if (!pRefr || !pRefr->baseForm)
            continue;

        char name[256];
        sprintf_s(name, std::size(name), "%s (%x)", pRefr->baseForm->GetName(), formComponent.Id);

        if (ImGui::Selectable(name, m_formId == formComponent.Id))
            m_formId = formComponent.Id;

        if(m_formId == formComponent.Id)
            s_selected = i;

        ++i;
    }

    ImGui::EndChild();
}

void TestService::DisplayEntityPanel(entt::entity aEntity) noexcept
{
    const auto pFormIdComponent = m_world.try_get<FormIdComponent>(aEntity);
    const auto pLocalComponent = m_world.try_get<LocalComponent>(aEntity);
    const auto pRemoteComponent = m_world.try_get<RemoteComponent>(aEntity);

    if (pFormIdComponent)               DisplayFormComponent(*pFormIdComponent);
    if (pLocalComponent)                DisplayLocalComponent(*pLocalComponent);
    if (pRemoteComponent)               DisplayRemoteComponent(*pRemoteComponent);
}

void TestService::DisplayFormComponent(FormIdComponent& aFormComponent) const noexcept
{
    if (!ImGui::CollapsingHeader("Form Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    const auto pActor = Cast<Actor>(TESForm::GetById(aFormComponent.Id));

    if (!pActor)
        return;

    ImGui::InputInt("Game Id", (int*)&aFormComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputFloat3("Position", pActor->position.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Rotation", pActor->rotation.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    int isDead = int(pActor->IsDead());
    ImGui::InputInt("Is dead?", &isDead, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    int isWeaponDrawn = int(pActor->actorState.IsWeaponDrawn());
    ImGui::InputInt("Is weapon drawn?", &isWeaponDrawn, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
#if TP_SKYRIM64
    float attributes[3] {pActor->GetActorValue(24), pActor->GetActorValue(25), pActor->GetActorValue(26)};
    ImGui::InputFloat3("Attributes (H/M/S)", attributes, "%.3f", ImGuiInputTextFlags_ReadOnly);
#endif
}

void TestService::DisplayLocalComponent(LocalComponent& aLocalComponent) const noexcept
{
    if (!ImGui::CollapsingHeader("Local Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    auto& action = aLocalComponent.CurrentAction;
    ImGui::InputInt("Net Id", (int*)&aLocalComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("Action Id", (int*)&action.ActionId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("Idle Id", (int*)&action.IdleId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalarN("State", ImGuiDataType_U32, &action.State1, 2, nullptr, nullptr, "%x", ImGuiInputTextFlags_ReadOnly);
}

void TestService::DisplayRemoteComponent(RemoteComponent& aLocalComponent) const noexcept
{
    if (!ImGui::CollapsingHeader("Remote Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::InputInt("Server Id", (int*)&aLocalComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
}
