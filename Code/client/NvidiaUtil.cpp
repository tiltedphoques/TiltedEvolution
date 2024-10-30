#include "NvidiaUtil.h"
#include <d3d11.h>

bool IsNvidiaOverlayLoaded()
{
    return GetModuleHandleW(L"nvspcap64.dll");
}

// This makes the Nvidia overlay happy.
// The call to `D3D11CreateDevice` probably causes some of their
// internal hooks to be called and do the required init work before the game window opens.
//
// We do dynamic invocation in order not to link `d3d11.dll` at compile-time
HRESULT CreateEarlyDxDevice(ID3D11Device** appOutDevice, D3D_FEATURE_LEVEL* apOutFeatureLevel)
{
    HMODULE d3d11Module = GetModuleHandleW(L"d3d11.dll");
    if (!d3d11Module)
        return E_FAIL;

    const auto pD3D11CreateDeviceFn =
        reinterpret_cast<decltype(&D3D11CreateDevice)>(GetProcAddress(d3d11Module, "D3D11CreateDevice"));

    return pD3D11CreateDeviceFn(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                                appOutDevice, apOutFeatureLevel, nullptr);
}
