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
    char pad[8];
    bool bFullScreenDisplay;
    bool bBorderlessDisplay;
    char pad2[22 - 16];
    bool bForce;
};

static_assert(offsetof(ViewportConfig, name) == 32);

struct BGSRenderer
{

    static BGSRenderer* Get();
    static ID3D11Device* GetDevice();

    char pad[28];
    DWORD unk;
    bool unk2;
    bool isWindowedMode;
    char pad0[4];
    DWORD syncInterval;
    char pad1[14];
    ID3D11Device* pD3dDevice;
    ID3D11DeviceContext* pD3dContext;
    HWND windowHandle;
    char pad2[8];
    DWORD windowWidth;
    DWORD windowHeight;
    IDXGISwapChain* pSwapChain;
};

static_assert(sizeof(ViewportConfig) == 40);
static_assert(offsetof(BGSRenderer, pD3dDevice) == 56);
static_assert(offsetof(BGSRenderer, pSwapChain) == 96);

#pragma pack(pop)
