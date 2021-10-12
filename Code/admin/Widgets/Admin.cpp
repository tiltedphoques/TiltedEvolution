#include "Admin.h"
#include <imgui.h>
#include <AdminApp.h>

Admin::Admin()
{
}

bool Admin::OnEnable()
{
    return true;
}

bool Admin::OnDisable()
{
    return true;
}

void Admin::Update(AdminApp& aApp)
{
    if (ImGui::Button("Shutdown Server"))
        ImGui::OpenPopup("Shutdown Server Dialog");

    if (ImGui::BeginPopupModal("Shutdown Server Dialog"))
    {
        ImGui::Text("Are you sure you want to shutdown the server?");

        if (ImGui::Button("Yes"))
        {
            aApp.SendShutdownRequest();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}
