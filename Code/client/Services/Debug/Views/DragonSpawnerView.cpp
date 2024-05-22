#include <imgui.h>
#include <inttypes.h>

void DebugService::DrawDragonSpawnerView()
{
#if TP_SKYRIM64
    ImGui::Begin("Dragon spawner");

    if (ImGui::CollapsingHeader("Dragons (level 10)"))
    {
        if (ImGui::Button("Brown (fire)"))
        {
            Actor::Spawn(0x1CA03);
        }
        if (ImGui::Button("Brown (frost)"))
        {
            Actor::Spawn(0xF80FA);
        }
        if (ImGui::Button("White (frost)"))
        {
            Actor::Spawn(0x8BC7F);
        }
        if (ImGui::Button("Bronze (frost)"))
        {
            Actor::Spawn(0x8BC7E);
        }
    }

    if (ImGui::CollapsingHeader("Blood dragons (level 20)"))
    {
        if (ImGui::Button("Blood (fire)"))
        {
            Actor::Spawn(0xF80FD);
        }
        if (ImGui::Button("Blood (frost)"))
        {
            Actor::Spawn(0xF77F8);
        }
    }

    if (ImGui::CollapsingHeader("Frost dragons (level 27-35)"))
    {
        if (ImGui::Button("Frost"))
        {
            Actor::Spawn(0x351C3);
        }
    }

    if (ImGui::CollapsingHeader("Elder dragons (level 36-44)"))
    {
        if (ImGui::Button("Elder (fire)"))
        {
            Actor::Spawn(0xF811B);
        }
        if (ImGui::Button("Elder (frost)"))
        {
            Actor::Spawn(0xF811A);
        }
    }

    if (ImGui::CollapsingHeader("Ancient dragons (level 45-54)"))
    {
        if (ImGui::Button("Ancient (fire)"))
        {
            Actor::Spawn(0xF811C);
        }
        if (ImGui::Button("Ancient (frost)"))
        {
            Actor::Spawn(0xF811E);
        }
    }

    if (ImGui::CollapsingHeader("Serpentine dragons (level 55-58)"))
    {
        if (ImGui::Button("Serpentine (fire)"))
        {
            Actor::Spawn(0x04036134);
        }
        if (ImGui::Button("Serpentine (frost)"))
        {
            Actor::Spawn(0x04036133);
        }
    }

    if (ImGui::CollapsingHeader("Revered dragons (level 62)"))
    {
        if (ImGui::Button("Revered (fire)"))
        {
            Actor::Spawn(0x02008431);
        }
    }

    if (ImGui::CollapsingHeader("Legendary dragons (level 75)"))
    {
        if (ImGui::Button("Legendary (fire)"))
        {
            Actor::Spawn(0x0200C5F5);
        }
        if (ImGui::Button("Legendary (frost)"))
        {
            Actor::Spawn(0x0200C5FD);
        }
    }

    if (ImGui::CollapsingHeader("Skeletal dragons (warning: very scary)"))
    {
        if (ImGui::Button("Skeletal (frost)"))
        {
            Actor::Spawn(0x9192C);
        }
    }

    ImGui::End();
#endif
}
