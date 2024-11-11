#include <Services/DebugService.h>
#include <Services/KeybindService.h>

#include "World.h"

#include <imgui.h>

TiltedPhoques::String ConvertWstringToString(const TiltedPhoques::WString& acWideString)
{
    if (acWideString.empty())
    {
        return {};
    }

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, acWideString.c_str(), static_cast<int>(acWideString.length()), NULL, 0, NULL, NULL);

    TiltedPhoques::String str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, acWideString.c_str(), static_cast<int>(acWideString.length()), &str[0], size_needed, NULL, NULL);

    return str;
}

const TiltedPhoques::WString& BindKey(const TiltedPhoques::WString& acKeyName, bool& aBindActive, const bool& acDebugKey)
{
    auto& keybindService = World::Get().GetKeybindService();
    TiltedPhoques::WString keyName = acKeyName;

    ImGui::SameLine(0);
    aBindActive = true;

    for (uint16_t key = 255; key > 1; key--)
    {
        if (GetAsyncKeyState(key) & 0x8000)
        {
            if (key == VK_ESCAPE)
            {
                aBindActive = false;
                break;
            }
            if (key == VK_LBUTTON || key == VK_RBUTTON)
            {
                continue;
            }

            aBindActive = false;

            if (acDebugKey)
                keybindService.BindDebugKey(key);
            else
                keybindService.BindUIKey(key);

            break;
        }
    }

    if (aBindActive)
        ImGui::Text("Press a key...");
    else
    {
        const auto& narrowString = ConvertWstringToString(keyName);
        ImGui::Text("%s", narrowString.c_str());
    }

    return {keyName};
}

void DebugService::DrawKeybindView()
{
#if TP_SKYRIM64
    ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_FirstUseEver);
    ImGui::Begin("Keybinds");

    TiltedPhoques::WString uiKeyName = World::Get().GetInputService().GetUIKey().first;

    if (IsRebinding())
        ImGui::Text("Press Escape to cancel");

    if (ImGui::Button("Show/Hide STR UI", ImVec2(200, 30)) || m_rebindUI)
    {
        m_rebindUI = true;

        uiKeyName = BindKey(uiKeyName, m_rebindUI, false);
    }
    else
    {
        ImGui::SameLine(0);
        const auto& narrowString = ConvertWstringToString(uiKeyName);
        ImGui::Text("%s", narrowString.c_str());

        m_rebindUI = false;
    }

    TiltedPhoques::WString debugKeyName = World::Get().GetDebugService().GetDebugKey().first;

    if (ImGui::Button("Show/Hide Debug UI", ImVec2(200, 30)) || m_rebindDebug)
    {
        m_rebindDebug = true;

        debugKeyName = BindKey(debugKeyName, m_rebindDebug, true);
    }
    else
    {
        ImGui::SameLine(0);
        const auto& narrowString = ConvertWstringToString(debugKeyName);
        ImGui::Text("%s", narrowString.c_str());

        m_rebindDebug = false;
    }

    ImGui::End();
#endif
}
