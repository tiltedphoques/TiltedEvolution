// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl.h>
#include <VersionHelpers.h>
#include <Windows.h>

#include "SupportChecks.h"

namespace oobe
{
namespace
{
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// TODO: requires some logging.
bool TestD3D11Support()
{
    // FIXME(Force): Ignore amd for now
    if (GetModuleHandleW(L"atiuxp64.dll"))
        return true;

    ComPtr<ID3D11Device> device;

    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                                   device.GetAddressOf(), nullptr, nullptr);

    if (FAILED(hr))
        return false;

    ComPtr<IDXGIDevice> dxgiOrig;
    hr = device.As(&dxgiOrig);

    if (FAILED(hr))
    {
        return true;
    }

    ComPtr<IDXGIDevice2> dxgiCheck;
    hr = dxgiOrig.As(&dxgiCheck);

    return SUCCEEDED(hr);
}
} // namespace

// D3D11 Test

CompatabilityStatus ReportModCompatabilityStatus()
{
    if (!IsWindows8Point1OrGreater())
        return CompatabilityStatus::kOldOS;

    /*
    if (!TestD3D11Support())
        return CompatabilityStatus::kDX11Unsupported;
    */

    return CompatabilityStatus::kAllGood;
}
} // namespace oobe
