// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_6.h>

#include <set>
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>

#include <VersionHelpers.h>
#include <Windows.h>
#include <wrl.h>

#include "SupportChecks.h"

// TODO(Force): Try refactoring this
// into 
UINT g_SuggestedDeviceByLauncher = 0;

namespace oobe
{
namespace
{
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

std::string GetCPUName()
{
    int CPUInfo[4] = {-1};
    unsigned nExIds, i = 0;
    char CPUBrandString[0x40]{};
    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    return CPUBrandString;
}

using TDeviceSet =  std::set<std::pair<size_t, UINT>>;
void SelectBestVideocard(IDXGIFactory1 *apFactory, TDeviceSet& aDevices)
{
    ComPtr<IDXGIAdapter1> pAdapter;
    for (UINT i = 0; apFactory->EnumAdapters1(i, pAdapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc{};
        pAdapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        // use GB instead of GiB in order to not confuse users.
        static auto makeGB = [](size_t byteCount) -> float { return ((byteCount / 1000) / 1000) / 1000; };
        spdlog::info(L"GPU {} = {} with {} GB VRAM", i, desc.Description, makeGB(desc.DedicatedVideoMemory));

        aDevices.insert(std::make_pair(desc.DedicatedVideoMemory, i));
    }
}

bool TestD3D11Support()
{
    return true;

    ComPtr<IDXGIFactory3> pFactory;
    if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&pFactory))))
    {
        spdlog::error("CreateDXGIFactory failed");
        return false;
    }

    TDeviceSet videoCards;
    SelectBestVideocard(pFactory.Get(), videoCards);

    ComPtr<IDXGIAdapter1> pAdapter = nullptr;
    // the last entry of the set contains the desired(best) index.
    g_SuggestedDeviceByLauncher = videoCards.rbegin()->second;
    pFactory->EnumAdapters1(g_SuggestedDeviceByLauncher, pAdapter.ReleaseAndGetAddressOf());

    DXGI_ADAPTER_DESC1 desc{};
    pAdapter->GetDesc1(&desc);
    spdlog::info(L"Selected {} as device (most VRAM)", desc.Description);

    ComPtr<ID3D11Device> device;
    HRESULT hr = D3D11CreateDevice(pAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                                   device.GetAddressOf(), nullptr, nullptr);
    if (FAILED(hr))
    {
        spdlog::error("D3D11CreateDevice failed");
        return false;
    }

    ComPtr<IDXGIDevice> dxgiOrig;
    hr = device.As(&dxgiOrig);

    if (FAILED(hr))
    {
        spdlog::error("As IDXGIDevice failed");
        return true;
    }

    ComPtr<IDXGIDevice2> dxgiCheck;
    hr = dxgiOrig.As(&dxgiCheck);
    if (FAILED(hr))
    {
        spdlog::error("As IDXGIDevice2 failed");
    }

    return SUCCEEDED(hr);
}
} // namespace

typedef void(WINAPI* RtlGetVersion_FUNC)(OSVERSIONINFOEXW*);

CompatabilityStatus ReportModCompatabilityStatus()
{
    // use an api that isnt subject to manifestation yet.
    auto* pfRtlGetVersion =
        reinterpret_cast<RtlGetVersion_FUNC>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion"));
    if (!IsWindows8Point1OrGreater() || !pfRtlGetVersion)
        return CompatabilityStatus::kOldOS;

    OSVERSIONINFOEXW osInfo{};
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    pfRtlGetVersion(&osInfo);
    // since windows lies to us.
    if (osInfo.dwBuildNumber >= 22000)
        osInfo.dwMajorVersion = 11;
    spdlog::info("OS = {}.{}.{}", osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber);
    spdlog::info("CPU = {}", GetCPUName());

    if (!TestD3D11Support())
        return CompatabilityStatus::kDX11Unsupported;

    return CompatabilityStatus::kAllGood;
}
} // namespace oobe
