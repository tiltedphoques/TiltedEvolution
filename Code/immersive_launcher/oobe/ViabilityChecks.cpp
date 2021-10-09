// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "oobe/ViabilityChecks.h"

#include <Windows.h>
#include <VersionHelpers.h>
#include <TiltedCore/Filesystem.hpp>

namespace oobe
{
    // D3D11 Test

bool TestPlatformViability(Policy apolicyLevel)
{
    // Test hardware
    if (!IsWindows8OrGreater() && apolicyLevel != Policy::kLax)
    {
        return false;
    }

    // TODO: should also query hardware support for AES

    // Mainly to prevent idiocy of force
#if (CLIENT_DLL)
    // Test install
    auto cefDll = TiltedPhoques::GetPath() / L"libcef.dll";
    if (!std::filesystem::exists(cefDll))
    {
        __debugbreak();
        return false;
    }
#endif

    return true;
}
} // namespace oobe
