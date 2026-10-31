// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <string>

namespace Console
{
class ConsoleRegistry;

// Maps a setting name to the environment variable that overrides it.
// The variable is the setting's key (the part after the "Section:" prefix)
// in upper case, e.g. "Gameplay:uDifficulty" becomes "UDIFFICULTY" and
// "GameServer:sPassword" becomes "SPASSWORD".
std::string ToEnvironmentVariableName(const char* apSettingName);

// Applies every set environment variable on top of the settings in the
// registry, so environment values win over the ini file and the defaults.
void LoadSettingsFromEnvironment(ConsoleRegistry& aReg);
} // namespace Console
