// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <TiltedCore/Filesystem.hpp>
#include <Windows.h>

#include "utils/Error.h"
#include "utils/Registry.h"

#include "InstallCheckFlow.h"
#include "SmartPathSelection.h"

namespace oobe
{
bool CheckInstall(launcher::LaunchContext& LC, bool needsSelect)
{
    if (!SelectInstall(needsSelect))
    {
        Die("Failed to select game install.");
        return false;
    }

    if (LC.gamePath == TiltedPhoques::GetPath())
    {
        Die("Please uninstall mod from game directory.");
        return false;
    }

    return true;
}
} // namespace oobe
