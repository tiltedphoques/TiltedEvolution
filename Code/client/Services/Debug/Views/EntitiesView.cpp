#include <Services/DebugService.h>

#include <Services/CharacterService.h>

#include <AI/AIProcess.h>
#include <PlayerCharacter.h>
#include <Games/ActorExtension.h>
#include <Forms/TESObjectCELL.h>

#include <Messages/RequestSpawnData.h>

#include <Events/MoveActorEvent.h>

#include <World.h>
#include <imgui.h>
#include <inttypes.h>

void DebugService::DrawEntitiesView()
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

void DebugService::DisplayEntities() noexcept
{
    static uint32_t s_selected = 0;

    StackAllocator<1 << 12> allocator;
    ScopedAllocator _{ allocator };

    const auto view = m_world.view<FormIdComponent>(entt::exclude<ObjectComponent>);

    Vector<entt::entity> entities(view.begin(), view.end());

    ImGui::Text("Actor list (%d)", entities.size());

    ImGui::BeginChild("Entities", ImVec2(0, 200), true);

    int i = 0;
    for(auto it : entities)
    {
        auto& formComponent = view.get<FormIdComponent>(it);
        const auto pActor = Cast<Actor>(TESForm::GetById(formComponent.Id));

        if (!pActor)
            continue;

        char name[256];

        if (!pActor->baseForm)
            strncpy_s(name, "UNNAMED", sizeof(name));

        sprintf_s(name, std::size(name), "%s (%x)", pActor->baseForm->GetName(), formComponent.Id);

        if (ImGui::Selectable(name, m_formId == formComponent.Id))
            m_formId = formComponent.Id;

        if(m_formId == formComponent.Id)
            s_selected = i;

        ++i;
    }

    ImGui::EndChild();

    if (s_selected < entities.size())
        DisplayEntityPanel(entities[s_selected]);
}

void DebugService::DisplayObjects() noexcept
{
    static uint32_t s_selected = 0;

    StackAllocator<1 << 12> allocator;
    ScopedAllocator _{ allocator };

    const auto view = m_world.view<FormIdComponent, ObjectComponent>();

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

void DebugService::DisplayEntityPanel(entt::entity aEntity) noexcept
{
    const auto pFormIdComponent = m_world.try_get<FormIdComponent>(aEntity);
    const auto pLocalComponent = m_world.try_get<LocalComponent>(aEntity);
    const auto pRemoteComponent = m_world.try_get<RemoteComponent>(aEntity);

    if (pFormIdComponent)               DisplayFormComponent(*pFormIdComponent);
    if (pLocalComponent)                DisplayLocalComponent(*pLocalComponent, pFormIdComponent ? pFormIdComponent->Id : 0);
    if (pRemoteComponent)               DisplayRemoteComponent(*pRemoteComponent, aEntity, pFormIdComponent ? pFormIdComponent->Id : 0);
}

void DebugService::DisplayFormComponent(FormIdComponent& aFormComponent) const noexcept
{
    if (!ImGui::CollapsingHeader("Form Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    const auto pActor = Cast<Actor>(TESForm::GetById(aFormComponent.Id));

    if (!pActor)
        return;

    /*
    if (ImGui::Button("Teleport away"))
    {
        m_world.GetRunner().Queue([id = aFormComponent.Id]() {
            Actor* pActor = Cast<Actor>(TESForm::GetById(id));
            TESObjectCELL* pCell = Cast<TESObjectCELL>(TESForm::GetById(0x133C6));
            NiPoint3 pos{};
            pos.x = -313.f;
            pos.y = 4.f;
            pos.z = 13.f;
            pActor->MoveTo(pCell, pos);
        });
    }
    */

    ImGui::InputInt("Game Id", (int*)&aFormComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputFloat3("Position", pActor->position.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Rotation", pActor->rotation.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    int isDead = int(pActor->IsDead());
    ImGui::InputScalar("Is dead?", ImGuiDataType_U8, &isDead, 0, 0, "%" PRIx8, ImGuiInputTextFlags_ReadOnly);
    int isRemote = int(pActor->GetExtension()->IsRemote());
    ImGui::InputScalar("Is remote?", ImGuiDataType_U8, &isRemote, 0, 0, "%" PRIx8, ImGuiInputTextFlags_ReadOnly);
    auto handle = pActor->GetHandle();
    ImGui::InputInt("Handle", (int*)&handle.handle.iBits, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
#if TP_SKYRIM64
    auto owner = pActor->GetCommandingActor();
    int commandingActorId = int(owner ? owner->formID : 0x0);
    ImGui::InputScalar("Commanding Actor", ImGuiDataType_U8, &commandingActorId, 0, 0, "%" PRIx8, ImGuiInputTextFlags_ReadOnly);
    float attributes[3] {pActor->GetActorValue(24), pActor->GetActorValue(25), pActor->GetActorValue(26)};
    ImGui::InputFloat3("Attributes (H/M/S)", attributes, "%.3f", ImGuiInputTextFlags_ReadOnly);
#endif
}

void DebugService::DisplayLocalComponent(LocalComponent& aLocalComponent, const uint32_t acFormId) const noexcept
{
    if (!ImGui::CollapsingHeader("Local Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (ImGui::Button("Teleport to me"))
    {
        auto* pPlayer = PlayerCharacter::Get();
        m_world.GetRunner().Trigger(MoveActorEvent(acFormId, pPlayer->parentCell->formID, pPlayer->position));
    }

    auto& action = aLocalComponent.CurrentAction;
    ImGui::InputInt("Net Id", (int*)&aLocalComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("Action Id", (int*)&action.ActionId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("Idle Id", (int*)&action.IdleId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalarN("State", ImGuiDataType_U32, &action.State1, 2, nullptr, nullptr, "%x", ImGuiInputTextFlags_ReadOnly);
}

void DebugService::DisplayRemoteComponent(RemoteComponent& aRemoteComponent, const entt::entity acEntity, const uint32_t acFormId) const noexcept
{
    if (!ImGui::CollapsingHeader("Remote Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::InputInt("Server Id", (int*)&aRemoteComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Take ownership"))
    {
        m_world.GetRunner().Queue([acEntity, acFormId]() {
            if (auto* pRemoteCompoment = World::Get().try_get<RemoteComponent>(acEntity))
                World::Get().GetCharacterService().TakeOwnership(acFormId, pRemoteCompoment->Id, acEntity);
        });
    }

    if (ImGui::Button("Get spawn data"))
    {
        m_world.GetRunner().Queue([this, acEntity, acFormId]() {
            if (auto* pRemoteCompoment = World::Get().try_get<RemoteComponent>(acEntity))
            {
                RequestSpawnData request{};
                request.Id = pRemoteCompoment->Id;
                m_transport.Send(request);
            }
        });
    }
}
