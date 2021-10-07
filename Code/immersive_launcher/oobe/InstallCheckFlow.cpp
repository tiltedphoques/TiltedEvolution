// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <Windows.h>
#include "utils/Error.h"
#include "SmartPathSelection.h"

namespace oobe
{
    bool CheckInstall()
    {
        bool keyPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        if (!SelectInstall(keyPressed))
        {
            Die("Failed to select game install.");
            return false;
        }

        return true;
    }
}
