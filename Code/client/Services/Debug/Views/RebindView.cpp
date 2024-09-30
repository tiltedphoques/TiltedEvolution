#include "DInputHook.hpp"

#include <Services/DebugService.h>
#include <Services/RebindService.h>

#include "World.h"

#include <imgui.h>

const TiltedPhoques::String& BindKey(const TiltedPhoques::String& aKeyName, bool& aBindActive)
{
    auto& rebindService = World::Get().GetRebindService();
    TiltedPhoques::String keyName = aKeyName;

    ImGui::SameLine(0);
    aBindActive = true;

    for (int key = 255; key > 1; key--)
    {
        if (GetAsyncKeyState(key) & 0x8000)
        {
            if (key == VK_ESCAPE || key == VK_LBUTTON || key == VK_RBUTTON)
            {
                aBindActive = false;
                break;
            }

            aBindActive = false;
            rebindService.BindNewKey(key);

            break;
        }
    }

    if (aBindActive)
    {
        ImGui::Text("Press a key...");
    }
    else
    {
        ImGui::Text(keyName.c_str());
    }

    return keyName;
}

void DebugService::DrawRebindView()
{
#if TP_SKYRIM64
    ImGui::SetNextWindowSize(ImVec2(250, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin("Rebind");


    if (ImGui::CollapsingHeader("UI"))
    {
        const auto& uiKey = World::Get().GetInputService().GetUIKey();
        TiltedPhoques::String keyName = uiKey.first;
        spdlog::info("{}", uiKey.first);

        if (ImGui::Button("Open/Close", ImVec2(100, 30)) || m_rebindActive)
        {
            m_rebindActive = true;

            keyName = BindKey(uiKey.first, m_rebindActive);
        }
        else
        {
            ImGui::SameLine(0);
            ImGui::Text("%s", keyName.c_str());

            m_rebindActive = false;
        }
    }

    if (ImGui::CollapsingHeader("Debug"))
    {

    }

    ImGui::End();
#endif
}
