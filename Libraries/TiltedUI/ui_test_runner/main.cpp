
#include "GraphicsRenderer.h"

#include <spdlog/spdlog.h>
#include <OverlayApp.h>
#include <OverlayRenderHandler.hpp>
#include <OverlayRenderHandlerD3D11.h>

#include <TiltedCore/Stl.hpp>

struct OverlayClient : TiltedPhoques::OverlayClient
{
    OverlayClient(TiltedPhoques::OverlayRenderHandler* apHandler);
    virtual ~OverlayClient() noexcept;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

    TP_NOCOPYMOVE(OverlayClient);

private:
};

OverlayClient::OverlayClient(TiltedPhoques::OverlayRenderHandler* apHandler)
    : TiltedPhoques::OverlayClient(apHandler)
{
}

OverlayClient::~OverlayClient() noexcept
{
}

bool OverlayClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
    CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    if (message->GetName() == "ui-event")
    {
        auto pArguments = message->GetArgumentList();

        auto eventName = pArguments->GetString(0).ToString();
        auto eventArgs = pArguments->GetList(1);

        if (eventName == "connect")
        {
            
        }
        if (eventName == "disconnect")
        {
            
        }

        return true;
    }

    return false;
}

class UITestRunner;

class D3D11RenderProvider final : TiltedPhoques::OverlayApp::RenderProvider, TiltedPhoques::OverlayRenderHandlerD3D11::Renderer
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
        GraphicsRenderer::Initialize(GetModuleHandleW(nullptr));

        m_pProvider = TiltedPhoques::MakeUnique<D3D11RenderProvider>(*this);
        m_pOverlay = new TiltedPhoques::OverlayApp((TiltedPhoques::OverlayApp::RenderProvider*)m_pProvider.get(), new ::OverlayClient(m_pProvider->Create()));

        if (!m_pOverlay->Initialize("http://www.http2demo.io/"))
            __debugbreak();

        m_pOverlay->GetClient()->Create();
    }

    bool Draw()
    {
        if (GetAsyncKeyState(VK_F3) & 1) {
            m_bShowCEF = !m_bShowCEF;
            UpdateWindowTitle();

            m_pOverlay->GetClient()->Reset();
        }

        if (GetAsyncKeyState(VK_F4) & 1) {
            m_pOverlay->ShowDevTools();
        }

        if (GetAsyncKeyState(VK_F5) & 1) {
            GraphicsRenderer::ToggleRect();
        }

        return GraphicsRenderer::Run(); 
    }

    void DrawWithin() override
    {
        if (m_bShowCEF) {
            m_pOverlay->GetClient()->Render();
        }
    }

    void UpdateWindowTitle()
    {
        auto title = fmt::format("UITestRunner{}", m_bShowCEF ? " - Cef active" : "");
        SetWindowTextA(m_hwnd, title.c_str());
    }

private:
    CefRefPtr<TiltedPhoques::OverlayApp> m_pOverlay{ nullptr };
    TiltedPhoques::UniquePtr<D3D11RenderProvider> m_pProvider;
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
        CoInitialize(nullptr);
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
