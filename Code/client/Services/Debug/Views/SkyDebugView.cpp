
#include <imgui.h>
#include <Services/TestService.h>
#include <Games/Skyrim/Shared/Sky/Sky.h>

void TestService::DrawSkyDebugView()
{
    auto* pInstance = Sky::GetInstance();
    if (!pInstance)
        return;

    ImGui::LabelText("A/ Climate: ", pInstance->GetCurrentClimate()->GetName());
    ImGui::LabelText("A/ Weather: ", pInstance->GetCurrentWeather()->GetName());

    if (ImGui::Button("Set Weather"))
    {
    
    }
}
