#include <Services/TestService.h>

#include <Forms/ActorValueInfo.h>

#include <imgui.h>

void TestService::DrawActorValuesView()
{
    Actor* pActor = RTTI_CAST(TESForm::GetById(m_formId), TESForm, Actor);
    if (!pActor)
        return;

    //for (int i = 0; i < ActorValueInfo::kActorValueCount; i++)
    {
        ActorValueOwner& actorValueOwner = pActor->actorValueOwner;
        float health[3] {actorValueOwner.GetValue(24), actorValueOwner.GetBaseValue(24),
                        actorValueOwner.GetPermanentValue(24)};
        ImGui::InputFloat3("Health (val/base/perm)", health, "%.3f", ImGuiInputTextFlags_ReadOnly);
    }
}
