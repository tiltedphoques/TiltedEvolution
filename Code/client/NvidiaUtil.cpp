#include "NvidiaUtil.h"
#include <d3d11.h>

bool IsNvidiaOverlayLoaded()
{
    return GetModuleHandleW(L"nvspcap64.dll");
}

// This makes the Nvidia overlay happy.
// The call to D3D11CreateDevice probably causes some of their
// internal hooks to be called and do the required init work before the game window opens.
HRESULT CreateEarlyDxDevice(ID3D11Device* apOutDevice, D3D_FEATURE_LEVEL* apOutFeatureLevel)
{
    return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &apOutDevice,
                             apOutFeatureLevel, nullptr);
}
