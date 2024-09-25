﻿#include "DInputHook.hpp"

#include <Services/DebugService.h>
#include <Services/RebindService.h>

#include "World.h"

#include <imgui.h>

void DebugService::DrawRebindView()
{
#if TP_SKYRIM64
    auto& inputService = World::Get().GetInputService();
    auto& rebindService = World::Get().GetRebindService();
    auto uiKey = inputService.GetUIKey();
    static bool s_bindingActive = false;

    ImGui::SetNextWindowSize(ImVec2(250, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin("Rebind");

    if (ImGui::CollapsingHeader("UI"))
    {
        if (ImGui::Button("Open/Close", ImVec2(100, 30)) || s_bindingActive)
        {
            ImGui::SameLine(0);
            s_bindingActive = true;

            if (s_bindingActive)
                ImGui::Text("Press a key...");
            else
                ImGui::Text(uiKey.first.c_str());

            // Loop through all virtual keys to find which key is pressed
            for (int key = 1; key < 256; key++)
            {
                if (GetAsyncKeyState(key) & 0x8000)
                {
                    if (key == VK_ESCAPE || key == VK_LBUTTON || key == VK_RBUTTON)
                        break;

                    auto& newKey = rebindService.GetKeyFromVKKeyCode(key);

                    if (!newKey.first.empty())
                    {
                        s_bindingActive = false;
                        uiKey = newKey;

                        if(rebindService.SetUIKey(uiKey))
                        {
                            TiltedPhoques::DInputHook::Get().SetToggleKeys({DIK_RCONTROL, static_cast<unsigned>(uiKey.second.diKeyCode)});
                            break;
                        }

                        spdlog::warn("{} key was not found", uiKey.first);
                    }
                }
            }
        }
        else
        {
            ImGui::SameLine(0);
            ImGui::Text("%s", uiKey.first.c_str());
        }
    }

    if (ImGui::CollapsingHeader("Debug"))
    {

    }

    ImGui::End();
#endif
}
