// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

namespace Console
{
class ConsoleRegistry;

void LoadSettingsFromCommand(ConsoleRegistry& aReg, int argc, char** argv);
} // namespace base
