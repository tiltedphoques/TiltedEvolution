#include <Utils/NvidiaUtil.h>
#include <d3d11.h>

bool IsNvidiaOverlayLoaded()
{
    return GetModuleHandleW(L"nvspcap64.dll");
}

// This makes the Nvidia overlay happy.
// The call to D3D11CreateDeviceAndSwapChain probably causes some of their
// internal hooks to be called and do the required init work before the game window opens.
HRESULT CreateEarlyDxSwapChain()
{
    ID3D11Device* device;
    return D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                                         nullptr, nullptr, &device, nullptr, nullptr);
}
