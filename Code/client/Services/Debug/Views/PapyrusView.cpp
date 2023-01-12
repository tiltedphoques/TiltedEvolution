#include <Services/DebugService.h>

#include <imgui.h>

void DebugService::OnPapyrusFunctionCallEvent(const PapyrusFunctionCallEvent& acEvent) noexcept
{
    std::lock_guard<std::mutex> _(m_papyrusEventLock);

    m_papyrusEvents.push_back(acEvent);
}

void DebugService::DrawPapyrusView()
{
    std::lock_guard<std::mutex> _(m_papyrusEventLock);

    ImGui::Begin("Papyrus");

    for (const auto& papyrusEvent : m_papyrusEvents)
    {
        String function = "";
        function += papyrusEvent.ScriptName;
        function += "::";
        function += papyrusEvent.FunctionName;
        function += "(";

        for (size_t i = 0; i < papyrusEvent.Arguments.size(); i++)
        {
            if (i != 0)
                function += ",";

            const auto& argument = papyrusEvent.Arguments[i];
            function += argument.Type;
            function += " ";
            function += argument.Name;
        }

        function += ")";

        ImGui::Text(function.c_str());
    }

    ImGui::End();
}
