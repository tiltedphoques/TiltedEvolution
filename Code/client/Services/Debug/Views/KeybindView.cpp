#include <Services/DebugService.h>
#include <Services/KeybindService.h>

#include "World.h"

#include <imgui.h>

const TiltedPhoques::String& BindKey(const TiltedPhoques::String& acKeyName, bool& aBindActive, const bool& acDebugKey)
{
    auto& rebindService = World::Get().GetKeybindService();
    TiltedPhoques::String keyName = acKeyName;

    ImGui::SameLine(0);
    aBindActive = true;

    for (uint16_t key = 255; key > 1; key--)
    {
        if (GetAsyncKeyState(key) & 0x8000)
        {
            if (key == VK_ESCAPE || key == VK_LBUTTON || key == VK_RBUTTON)
            {
                aBindActive = false;
                break;
            }

            aBindActive = false;

            if (acDebugKey)
                rebindService.BindDebugKey(key);
            else
                rebindService.BindUIKey(key);

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

void DebugService::DrawKeybindView()
{
#if TP_SKYRIM64
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Keybinds");

    ImGui::Text("UI");

    TiltedPhoques::String uiKeyName = World::Get().GetInputService().GetUIKey().first;

    if (ImGui::Button("Show/Hide", ImVec2(100, 30)) || m_rebindUI)
    {
        m_rebindUI = true;

        uiKeyName = BindKey(uiKeyName, m_rebindUI, false);
    }
    else
    {
        ImGui::SameLine(0);
        ImGui::Text("%s", uiKeyName.c_str());

        m_rebindUI = false;
    }

    ImGui::NewLine();
    ImGui::Text("Debug");

    TiltedPhoques::String debugKeyName = World::Get().GetDebugService().GetDebugKey().first;

    if (ImGui::Button("Show/Hide", ImVec2(100, 30)) || m_rebindDebug)
    {
        m_rebindDebug = true;

        debugKeyName = BindKey(debugKeyName, m_rebindDebug, true);
    }
    else
    {
        ImGui::SameLine(0);
        ImGui::Text("%s", debugKeyName.c_str());

        m_rebindDebug = false;
    }

    ImGui::End();
#endif
}
