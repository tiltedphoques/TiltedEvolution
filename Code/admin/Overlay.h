#pragma once

#include "widgets/Console.h"
#include "widgets/Admin.h"
#include "widgets/Widget.h"
#include <atomic>

struct Overlay
{
    Overlay();
    ~Overlay();

    Console& GetConsole();

    void Toggle();
    [[nodiscard]] bool IsEnabled() const noexcept;

    void Update(AdminApp& aApp);

  private:
    void SetActiveWidget(WidgetID aNewActive);

    WidgetID Toolbar();

    Console m_console;
    Admin m_admin;
    std::array<Widget*, size_t(WidgetID::COUNT)> m_widgets{};

    WidgetID m_activeWidgetID{WidgetID::CONSOLE};
    WidgetID m_nextActiveWidgetID{WidgetID::CONSOLE};

    std::atomic_bool m_enabled{false};
    std::atomic_bool m_toggled{false};
    bool m_initialized{false};
};
