
#include <Windows.h>
#include <assert.h>

#include "shader.inl"
#include "GraphicsRenderer.h"


GraphicsRenderer::GraphicsRenderer(HINSTANCE hs) : m_window(hs)
{

}

GraphicsRenderer::~GraphicsRenderer()
{

}

void GraphicsRenderer::Initialize() {
    InitD3D();
    CreateShaders();
}

void GraphicsRenderer::InitD3D()
{
    {
        ID3D11Device* baseDevice;
        ID3D11DeviceContext* baseDeviceContext;
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
            0, creationFlags,
            featureLevels, ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION, &baseDevice,
            0, &baseDeviceContext);
        if (FAILED(hResult)) {
            MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK);
            return;
        }

        // Get 1.1 interface of D3D11 Device and Context
        hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&m_pDevice);
        assert(SUCCEEDED(hResult));
        baseDevice->Release();

        hResult = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&m_pDeviceContext);
        assert(SUCCEEDED(hResult));
        baseDeviceContext->Release();
    }

#ifdef DEBUG
    // Set up debug layer to break on D3D11 errors
    ID3D11Debug* d3dDebug = nullptr;
    m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug);
    if (d3dDebug)
    {
        ID3D11InfoQueue* d3dInfoQueue = nullptr;
        if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
        {
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            d3dInfoQueue->Release();
        }
        d3dDebug->Release();
    }
#endif

    // Create Swap Chain
    {
        // Get DXGI Factory (needed to create Swap Chain)
        IDXGIFactory2* dxgiFactory;
        {
            IDXGIDevice1* dxgiDevice;
            HRESULT hResult = m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);
            assert(SUCCEEDED(hResult));

            IDXGIAdapter* dxgiAdapter;
            hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
            assert(SUCCEEDED(hResult));
            dxgiDevice->Release();

            DXGI_ADAPTER_DESC adapterDesc;
            dxgiAdapter->GetDesc(&adapterDesc);

            OutputDebugStringA("Graphics Device: ");
            OutputDebugStringW(adapterDesc.Description);

            hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);
            assert(SUCCEEDED(hResult));
            dxgiAdapter->Release();
        }

        DXGI_SWAP_CHAIN_DESC1 d3d11SwapChainDesc = {};
        d3d11SwapChainDesc.Width = 0; // use window width
        d3d11SwapChainDesc.Height = 0; // use window height
        d3d11SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        d3d11SwapChainDesc.SampleDesc.Count = 1;
        d3d11SwapChainDesc.SampleDesc.Quality = 0;
        d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        d3d11SwapChainDesc.BufferCount = 2;
        d3d11SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        d3d11SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        d3d11SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(m_pDevice, m_window.hwnd(), &d3d11SwapChainDesc, 0, 0, &m_pSwapchain);
        assert(SUCCEEDED(hResult));

        dxgiFactory->Release();
    }

    // Create Framebuffer Render Target
    {
        ID3D11Texture2D* d3d11FrameBuffer;
        HRESULT hResult = m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11FrameBuffer);
        assert(SUCCEEDED(hResult));

        hResult = m_pDevice->CreateRenderTargetView(d3d11FrameBuffer, 0, &m_pd3d11FrameBufferView);
        assert(SUCCEEDED(hResult));
        d3d11FrameBuffer->Release();
    }
}

void GraphicsRenderer::CreateShaders()
{
    // Create Vertex Shader
    ID3DBlob* vsBlob;
    {
        ID3DBlob* shaderCompileErrorsBlob;

        //std::wstring shaderPath(RUNNER_PATH);
        //shaderPath += LR"(\\UITestRunner\\shaders.hlsl)";

        HRESULT hResult = D3DCompile2((LPCVOID)kShaderSource, sizeof(kShaderSource), "triangleshader",
            nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, 0, nullptr, 0, &vsBlob, &shaderCompileErrorsBlob);

        //HRESULT hResult = D3DCompile2(shaderPath.c_str(), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vsBlob, &shaderCompileErrorsBlob);
        if (FAILED(hResult))
        {
            const char* errorString = NULL;
            if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                errorString = "Could not compile shader; file not found";
            else if (shaderCompileErrorsBlob) {
                errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                shaderCompileErrorsBlob->Release();
            }
            MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
            return;
        }

        hResult = m_pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &shaders.vertexShader);
        assert(SUCCEEDED(hResult));
    }

    // Create Pixel Shader
    {
        ID3DBlob* psBlob;
        ID3DBlob* shaderCompileErrorsBlob;

        HRESULT hResult = D3DCompile2((LPCVOID)kShaderSource, sizeof(kShaderSource), "triangleshader",
            nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, 0, nullptr, 0, &psBlob, &shaderCompileErrorsBlob);

        //HRESULT hResult = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &shaderCompileErrorsBlob);
        if (FAILED(hResult))
        {
            const char* errorString = NULL;
            if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                errorString = "Could not compile shader; file not found";
            else if (shaderCompileErrorsBlob) {
                errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                shaderCompileErrorsBlob->Release();
            }
            MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
            return;
        }

        hResult = m_pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &shaders.pixelShader);
        assert(SUCCEEDED(hResult));
        psBlob->Release();
    }

    // Create Input Layout
    {
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        HRESULT hResult = m_pDevice->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &shaders.inputLayout);
        assert(SUCCEEDED(hResult));
        vsBlob->Release();
    }

    // Create Vertex Buffer
    {
        float vertexData[] = { // x, y, r, g, b, a
            0.0f,  0.5f, 0.f, 1.f, 0.f, 1.f,
            0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f,
            -0.5f, -0.5f, 0.f, 0.f, 1.f, 1.f
        };
        shaders.stride = 6 * sizeof(float);
        shaders.numVerts = sizeof(vertexData) / shaders.stride;
        shaders.offset = 0;

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.ByteWidth = sizeof(vertexData);
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };

        HRESULT hResult = m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &shaders.vertexBuffer);
        assert(SUCCEEDED(hResult));
    }
}

void GraphicsRenderer::Resize() {
    RECT rc;
    GetClientRect(m_window.hwnd(), &rc);
    auto newWidth = rc.right - rc.left;
    auto newHeight = rc.bottom - rc.top;
    if (!newWidth || !newHeight)
        return;

    m_pDeviceContext->OMSetRenderTargets(0, 0, 0);
    m_pd3d11FrameBufferView->Release();

    DXGI_SWAP_CHAIN_DESC newMode{};
    m_pSwapchain->GetDesc(&newMode);

    HRESULT res = m_pSwapchain->ResizeBuffers(0, newWidth, newHeight, newMode.BufferDesc.Format, newMode.Flags);
    assert(SUCCEEDED(res));

    ID3D11Texture2D* d3d11FrameBuffer;
    res = m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11FrameBuffer);
    assert(SUCCEEDED(res));

    res = m_pDevice->CreateRenderTargetView(d3d11FrameBuffer, NULL,
        &m_pd3d11FrameBufferView);
    assert(SUCCEEDED(res));
    d3d11FrameBuffer->Release();
}

bool GraphicsRenderer::Run()
{
    MSG msg = {};
    while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            m_bIsRunning = false;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (m_window.resizing()) {
        Resize();
        m_window.ResetResized();
    }

    FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
    m_pDeviceContext->ClearRenderTargetView(m_pd3d11FrameBufferView, backgroundColor);

    RECT winRect;
    GetClientRect(m_window.hwnd(), &winRect);
    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(winRect.right - winRect.left), (FLOAT)(winRect.bottom - winRect.top), 0.0f, 1.0f };
    m_pDeviceContext->RSSetViewports(1, &viewport);

    // TODO: for the game we need to push and pop the pipeline, else we might never get stuff drawn!
    //m_pDeviceContext->OMSetRenderTargets(1, &m_pd3d11FrameBufferView, nullptr);
    DrawWithin();

    if (m_bDrawRectangle)
    {
        m_pDeviceContext->OMSetRenderTargets(1, &m_pd3d11FrameBufferView, nullptr);

        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_pDeviceContext->IASetInputLayout(shaders.inputLayout);
        m_pDeviceContext->VSSetShader(shaders.vertexShader, nullptr, 0);
        m_pDeviceContext->PSSetShader(shaders.pixelShader, nullptr, 0);
        m_pDeviceContext->IASetVertexBuffers(0, 1, &shaders.vertexBuffer, &shaders.stride, &shaders.offset);
        m_pDeviceContext->Draw(shaders.numVerts, 0);

    }

    //DrawWithin();
    m_pSwapchain->Present(1, 0);

    return m_bIsRunning;
}
