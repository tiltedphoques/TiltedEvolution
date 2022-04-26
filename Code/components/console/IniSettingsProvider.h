// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <filesystem>

namespace Console
{
class ConsoleRegistry;

void SaveSettingsToIni(ConsoleRegistry& aReg, const std::filesystem::path& path);
void LoadSettingsFromIni(ConsoleRegistry& aReg, const std::filesystem::path& path);
} // namespace base
