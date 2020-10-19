#pragma once
#pragma pack(push, 1)

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;

struct ViewportConfig
{
    char pad[8];
    HINSTANCE handle;
    WNDPROC wndProc;
    HICON hIcon;
    const char* name;
};

struct WindowConfig
{
    int32_t iSizeW;
    int32_t iSizeH;
    int32_t iLocationX;
    int32_t iLocationY;
    char pad[4];
    bool bFullScreenDisplay;
    bool bBorderlessDisplay;
    char pad2[22 - 16];
    bool bForce;
};

struct BGSRenderer
{

    static BGSRenderer* Get();
    static ID3D11Device* GetDevice();

    char pad0[64];
    DWORD syncInterval;
    char pad1[4];
    ID3D11Device* pD3dDevice;
    ID3D11DeviceContext* pD3dContext;
    HWND windowHandle;
    char pad2[8];
    DWORD windowWidth;
    DWORD windowHeight;
    IDXGISwapChain* pSwapChain;
};

static_assert(offsetof(BGSRenderer, windowHandle) == 88);
static_assert(offsetof(BGSRenderer, pD3dDevice) == 72);
static_assert(offsetof(BGSRenderer, pSwapChain) == 112);

#pragma pack(pop)
