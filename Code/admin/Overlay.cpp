#include "Overlay.h"
#include <imgui.h>
#include "AdminApp.h"

Console& Overlay::GetConsole()
{
    return m_console;
}

void Overlay::Toggle()
{
    if (!m_toggled)
        m_toggled = true;
}

bool Overlay::IsEnabled() const noexcept
{
    return m_initialized && m_enabled;
}

void Overlay::Update(AdminApp& aApp)
{
    if (m_toggled)
    {
        if (m_enabled)
        {
            if (m_widgets[static_cast<size_t>(m_activeWidgetID)]->OnDisable())
            {
                m_toggled = false;
                m_enabled = false;
            }
        }
        else
        {
            if (m_widgets[static_cast<size_t>(m_activeWidgetID)]->OnEnable())
            {
                m_toggled = false;
                m_enabled = true;
            }
        }
    }

    if (!m_enabled)
        return;

    const auto resolution = aApp.windowSize();

    ImGui::SetNextWindowPos(ImVec2(resolution.x() * 0.2f, resolution.y() * 0.2f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(resolution.x() * 0.6f, resolution.y() * 0.6f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(420, 315), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::Begin("Tools"))
    {
        const ImVec2 cZeroVec = {0, 0};

        SetActiveWidget(Toolbar());

        if (m_activeWidgetID == WidgetID::CONSOLE)
        {
            if (ImGui::BeginChild("Console", cZeroVec, true))
                m_console.Update(aApp);
            ImGui::EndChild();
        }
        else if (m_activeWidgetID == WidgetID::ADMIN)
        {
            if (ImGui::BeginChild("Admin", cZeroVec, true))
                m_admin.Update(aApp);
            ImGui::EndChild();
        }
    }
    ImGui::End();
}

Overlay::Overlay()
    : m_enabled(true)
{
    m_widgets[static_cast<size_t>(WidgetID::CONSOLE)] = &m_console;
    m_widgets[static_cast<size_t>(WidgetID::ADMIN)] = &m_admin;
}

Overlay::~Overlay()
{
}

void Overlay::SetActiveWidget(WidgetID aNewActive)
{
    if (aNewActive < WidgetID::COUNT)
        m_nextActiveWidgetID = aNewActive;

    if (m_activeWidgetID != m_nextActiveWidgetID)
    {
        assert(m_activeWidgetID < WidgetID::COUNT);
        if (m_widgets[static_cast<size_t>(m_activeWidgetID)]->OnDisable())
        {
            assert(m_nextActiveWidgetID < WidgetID::COUNT);
            if (m_widgets[static_cast<size_t>(m_nextActiveWidgetID)]->OnEnable())
                m_activeWidgetID = m_nextActiveWidgetID;
        }
    }
}

WidgetID Overlay::Toolbar()
{
    WidgetID activeID = WidgetID::COUNT;
    ImGui::SameLine();
    if (ImGui::Button("Console"))
        activeID = WidgetID::CONSOLE;
    ImGui::SameLine();
    if (ImGui::Button("Admin"))
        activeID = WidgetID::ADMIN;

    return activeID;
}
