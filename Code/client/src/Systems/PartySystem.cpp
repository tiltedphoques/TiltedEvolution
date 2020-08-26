
#include <imgui.h>
#include <Systems/PartySystem.h>
#include <Services/ImguiService.h>

PartySystem::PartySystem(entt::dispatcher& aDispatcher, ImguiService& aImguiService) noexcept
{
    m_drawConnection = aImguiService.OnDraw.connect<&PartySystem::OnDraw>(this);
}

void PartySystem::OnDraw() noexcept
{
    ImGui::Begin("PartyMan");
    if (ImGui::Button("Start Party"))
    {

    
    }

    ImGui::End();
}
