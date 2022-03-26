#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "NativeWindow.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

class GraphicsRenderer {
public:
    explicit GraphicsRenderer(HINSTANCE hs);
    ~GraphicsRenderer();

    void Initialize();
    bool Run();

    void ToggleRect() {
        m_bDrawRectangle = !m_bDrawRectangle;
    }

    NativeWindow& window() {
        return m_window;
    }

    virtual void DrawWithin() = 0;
private:
    void InitD3D();
    void CreateShaders();
    void Resize();

private:
    bool m_bIsRunning = true;
    bool m_bDrawRectangle = true;

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
    NativeWindow m_window;
};
