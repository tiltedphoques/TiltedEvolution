#pragma once

struct RenderSystemD3D9;
struct RenderSystemD3D11;

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

    entt::sink<TCallback> OnDraw;
    
private:

    entt::sigh<TCallback> m_drawSignal;
};
