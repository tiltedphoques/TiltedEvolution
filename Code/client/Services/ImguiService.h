#pragma once

#include <imgui/ImGuiDriver.h>

struct RenderSystemD3D9;
struct RenderSystemD3D11;
struct ImFont;

/**
 * @brief Draws the ImGui UI.
 */
struct ImguiService
{
    using TCallback = void();

    ImguiService();
    ~ImguiService() noexcept;

    TP_NOCOPYMOVE(ImguiService);

    void Create(RenderSystemD3D11* apRenderSystem, HWND aHwnd);

    void Render() const;
    void Reset() const;

    LRESULT WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void RawInputHandler(RAWINPUT& aRawinput);

    // Skyrim Together UI font for in-world name tags, or null if it couldn't be loaded.
    [[nodiscard]] ImFont* GetNameTagFont() const noexcept { return m_pNameTagFont; }

    entt::sink<entt::sigh<TCallback>> OnDraw;

private:
    void LoadNameTagFont();

    ImGuiImpl::ImGuiDriver m_imDriver;
    entt::sigh<TCallback> m_drawSignal;
    ImFont* m_pNameTagFont = nullptr;
};
