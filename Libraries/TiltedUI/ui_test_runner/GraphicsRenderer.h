#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

class GraphicsRenderer {
public:
    GraphicsRenderer();
    ~GraphicsRenderer();

    void Initialize(HINSTANCE hs);
    bool Run();

private:
    void CreateWindowX(HINSTANCE hs);
    void InitD3D();
    void CreateShaders();

private:
    bool m_bIsRunning = true;

    struct ShaderData {
        ID3D11Buffer* vertexBuffer = nullptr;
        UINT numVerts = 0;
        UINT stride = 0;
        UINT offset = 0;

        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11InputLayout* inputLayout = nullptr;
    } shaders;

protected:
    ID3D11Device1* m_pDevice = nullptr;
    ID3D11DeviceContext1* m_pDeviceContext = nullptr;
    IDXGISwapChain1* m_pSwapchain = nullptr;
    ID3D11RenderTargetView* m_pd3d11FrameBufferView = nullptr;

    HWND m_hwnd{};
};
