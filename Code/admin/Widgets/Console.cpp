#include "Console.h"
#include <imgui.h>

Console::Console()
{
}

bool Console::OnEnable()
{
    m_focusConsoleInput = true;
    return true;
}

bool Console::OnDisable()
{
    return true;
}

int Console::HandleConsoleHistory(ImGuiInputTextCallbackData* apData)
{
    auto* pConsole = static_cast<Console*>(apData->UserData);

    TiltedPhoques::String* pStr = nullptr;

    if (pConsole->m_newConsoleHistory)
    {
        pStr = &pConsole->m_consoleHistory[pConsole->m_consoleHistoryIndex];
    }
    else if (apData->EventKey == ImGuiKey_UpArrow && pConsole->m_consoleHistoryIndex > 0)
    {
        pConsole->m_consoleHistoryIndex--;

        pStr = &pConsole->m_consoleHistory[pConsole->m_consoleHistoryIndex];
    }
    else if (apData->EventKey == ImGuiKey_DownArrow && pConsole->m_consoleHistoryIndex + 1 < pConsole->m_consoleHistory.size())
    {
        pConsole->m_consoleHistoryIndex++;

        pStr = &pConsole->m_consoleHistory[pConsole->m_consoleHistoryIndex];
    }

    pConsole->m_newConsoleHistory = false;

    if (pStr)
    {
        std::memcpy(apData->Buf, pStr->c_str(), pStr->length() + 1);
        apData->BufDirty = true;
        apData->BufTextLen = pStr->length();
        apData->CursorPos = apData->BufTextLen;
    }

    return 0;
}

void Console::Update(AdminApp& aApp)
{
    ImGui::Checkbox("Clear Input", &m_inputClear);
    ImGui::SameLine();
    if (ImGui::Button("Clear Output"))
    {
        m_outputLines.clear();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Scroll Output", &m_outputShouldScroll);

    auto& style = ImGui::GetStyle();
    auto inputLineHeight = ImGui::GetTextLineHeight() + style.ItemInnerSpacing.y * 2;
    
    if (ImGui::ListBoxHeader("##ConsoleHeader", ImVec2(-1, -(inputLineHeight + style.ItemSpacing.y))))
    {
        ImGuiListClipper clipper;
        clipper.Begin(m_outputLines.size());
        while (clipper.Step())
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) 
            {
                auto& item = m_outputLines[i];
                ImGui::PushID(i);
                if (ImGui::Selectable(item.c_str()))
                {
                    auto str = item;
                    if (item[0] == '>' && item[1] == ' ')
                        str = str.substr(2);

                    std::strncpy(m_Command, str.c_str(), sizeof(m_Command) - 1);
                    m_focusConsoleInput = true;
                }
                ImGui::PopID();
            }

        if (m_outputScroll)
        {
            if (m_outputShouldScroll)
                ImGui::SetScrollHereY();
            m_outputScroll = false;
        }
        
        ImGui::ListBoxFooter();
    }
    
    if (m_focusConsoleInput)
    {
        ImGui::SetKeyboardFocusHere();
        m_focusConsoleInput = false;
    }
    ImGui::SetNextItemWidth(-FLT_MIN);
    const auto execute = ImGui::InputText("##InputCommand", m_Command, std::size(m_Command),
                                          ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory,  &HandleConsoleHistory, this);
    ImGui::SetItemDefaultFocus();
    if (execute)
    {
        m_consoleHistoryIndex = m_consoleHistory.size();
        m_consoleHistory.push_back(m_Command);
        m_newConsoleHistory = true;
        
        if (m_inputClear)
        {
            std::memset(m_Command, 0, sizeof(m_Command));
        }

        m_focusConsoleInput = true;
    }
}

void Console::Log(const TiltedPhoques::String& acpText)
{
    size_t first = 0;
    while (first < acpText.size())
    {
        const auto second = acpText.find_first_of('\n', first);

        if (second == std::string_view::npos)
        {
            m_outputLines.emplace_back(acpText.substr(first));
            break;
        }

        if (first != second)
            m_outputLines.emplace_back(acpText.substr(first, second-first));

        first = second + 1;
    }

    m_outputScroll = true;
}

bool Console::GameLogEnabled() const
{
    return !m_disabledGameLog;
}
