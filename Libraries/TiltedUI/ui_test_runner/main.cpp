
#include "GraphicsRenderer.h"

#include <spdlog/spdlog.h>
#include <NgApp.h>
#include <OverlayRenderHandler.h>
#include <OverlayRenderHandlerD3D11.h>

#include <TiltedCore/Stl.hpp>

class UITestRunner;

class D3D11RenderProvider final : TiltedPhoques::RenderProvider, TiltedPhoques::OverlayRenderHandlerD3D11::Renderer
{
public:
    explicit D3D11RenderProvider(UITestRunner& aRunner) : m_runner(aRunner) {}

    TiltedPhoques::OverlayRenderHandler* Create() override;
    HWND GetWindow() override;
    IDXGISwapChain* GetSwapChain() const noexcept override;

private:
    UITestRunner& m_runner;
};

class UITestRunner final : public GraphicsRenderer
{
    friend class D3D11RenderProvider;
public:
    UITestRunner(int argc, char** argv)
    {
        using namespace TiltedPhoques;

        GraphicsRenderer::Initialize(GetModuleHandleW(nullptr));

        m_pProvider = MakeUnique<D3D11RenderProvider>(*this);
        m_pDriver = new NgApp((RenderProvider*)m_pProvider.get());
        const NgApp::CreateInfo info{
            .pWorkerName = L"TPProcess.exe",
            .remoteDebugPort = 8384
        };
        if (!m_pDriver->Initialize(info))
            __debugbreak();

        m_pMainFrame = m_pDriver->CreateSpace();
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
        auto title = fmt::format("UITestRunner{}", m_bShowCEF ? " - Cef active" : "");
        SetWindowTextA(m_hwnd, title.c_str());
    }

private:
    CefRefPtr<TiltedPhoques::NgApp> m_pDriver{ nullptr };
    TiltedPhoques::UniquePtr<D3D11RenderProvider> m_pProvider;
    TiltedPhoques::NgSpace* m_pMainFrame{ nullptr };
    bool m_bShowCEF = false;
};

TiltedPhoques::OverlayRenderHandler* D3D11RenderProvider::Create()
{
    auto it = new TiltedPhoques::OverlayRenderHandlerD3D11(this);
    it->SetVisible(true);
    return it;
}

HWND D3D11RenderProvider::GetWindow()
{
    return m_runner.m_hwnd;
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

int main(int argc, char** argv) {
    ComScope cs;
    (void)cs;

    UITestRunner runner(argc, argv);
    while (true)
    {
        if (!runner.Draw()) break;
    }

    return 0;
}
