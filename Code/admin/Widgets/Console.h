#pragma once

#include "Widget.h"

#include <TiltedCore/Stl.hpp>

struct ImGuiInputTextCallbackData;

struct Console : Widget
{
    Console();
    ~Console() override = default;

    bool OnEnable() override;
    bool OnDisable() override;
    void Update(AdminApp& aApp) override;

    void Log(const TiltedPhoques::String& acpText);
    bool GameLogEnabled() const;

private:

    static int HandleConsoleHistory(ImGuiInputTextCallbackData* apData);

    TiltedPhoques::Vector<TiltedPhoques::String> m_outputLines{ };
    TiltedPhoques::Vector<TiltedPhoques::String> m_consoleHistory{};
    int64_t m_consoleHistoryIndex{ 0 };
    bool m_newConsoleHistory{true};
    bool m_outputShouldScroll{ true };
    bool m_outputScroll{ false };
    bool m_inputClear{ true };
    bool m_disabledGameLog{ true };
    bool m_focusConsoleInput{ false };
    char m_Command[0x10000]{ 0 };
};
