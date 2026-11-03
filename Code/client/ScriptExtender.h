#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

// Loads the installed F4SE or SKSE core before the game's CRT startup, letting
// the extender initialize its own plugins and scripting interfaces normally.
void LoadScriptExtender(const std::filesystem::path& acGamePath, const std::array<uint32_t, 4>& acRuntimeVersion);

// Check if the SE is active
bool IsScriptExtenderLoaded();
