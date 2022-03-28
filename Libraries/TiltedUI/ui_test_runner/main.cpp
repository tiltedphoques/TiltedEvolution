
#include "GraphicsRenderer.h"

#include <spdlog/spdlog.h>
#include <NgApp.h>
#include <NgRenderHandler.h>
#include <NgRenderHandlerD3D11.h>

#include <TiltedCore/Stl.hpp>

class UITestRunner;

class D3D11RenderProvider final : TiltedPhoques::RenderProvider, TiltedPhoques::NgRenderHandlerD3D11::Renderer
{
public:
    explicit D3D11RenderProvider(UITestRunner& aRunner) : m_runner(aRunner) {}

    TiltedPhoques::NgRenderHandler* Create() override;
    HWND GetWindow() override;
    IDXGISwapChain* GetSwapChain() const noexcept override;

private:
    UITestRunner& m_runner;
};

class UITestRunner final : public GraphicsRenderer
{
    friend class D3D11RenderProvider;
public:
    UITestRunner(HINSTANCE hs) : GraphicsRenderer(hs), m_provider(*this), m_App((TiltedPhoques::RenderProvider*)&m_provider)
    {
        using namespace TiltedPhoques;
        GraphicsRenderer::Initialize();

        const NgApp::Settings config{
         .pWorkerName = L"TPProcess.exe",
         .devtoolsPort = 8384,
         .useSharedResources = true,
         .disableCors = true, // as this is an internal tool, we can disable it for development.
         .instanceArgs = hs,
        };
        if (!m_App.Initialize(config))
            __debugbreak();

        m_pMainFrame = m_App.CreateSpace("https://madebyevan.com/webgl-water/");
    }

    bool Draw()
    {
        if (GetAsyncKeyState(VK_F3) & 1) {
            m_bShowCEF = !m_bShowCEF;
            UpdateWindowTitle();

            //m_pMainFrame->GetClient()->Reset();
        }

        if (GetAsyncKeyState(VK_F4) & 1) {
            m_pMainFrame->ShowDevTools();
        }

        if (GetAsyncKeyState(VK_F5) & 1) {
            GraphicsRenderer::ToggleRect();
        }

        return GraphicsRenderer::Run();
    }

    void DrawWithin() override
    {
        if (m_bShowCEF) {
            m_pMainFrame->GetClient()->Render();
        }
    }

    void UpdateWindowTitle()
    {
        auto title = fmt::format("NG_UI Runner{}", m_bShowCEF ? " - Cef active" : "");
        SetWindowTextA(m_window.hwnd(), title.c_str());
    }

private:
    D3D11RenderProvider m_provider;
    TiltedPhoques::NgApp m_App;
    TiltedPhoques::NgSpace* m_pMainFrame{ nullptr };
    bool m_bShowCEF = false;
};

TiltedPhoques::NgRenderHandler* D3D11RenderProvider::Create()
{
    auto it = new TiltedPhoques::NgRenderHandlerD3D11(this);
    it->SetVisible(true);
    return it;
}

HWND D3D11RenderProvider::GetWindow()
{
    return m_runner.window().hwnd();
}

IDXGISwapChain* D3D11RenderProvider::GetSwapChain() const noexcept
{
    return m_runner.m_pSwapchain;
}

struct ComScope
{
    ComScope()
    {
        (void)CoInitialize(nullptr);
    }
    ~ComScope()
    {
        CoUninitialize();
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    ComScope cs;
    (void)cs;

    UITestRunner runner(hInstance);
    while (true)
    {
        if (!runner.Draw()) break;
    }

    return 0;
}
