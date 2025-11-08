#pragma once

#include <string>

namespace stubs
{
// Returns true if the module name is found in the hard block list.
bool IsDllBlocked(std::wstring_view dllName);

// Returns true if the module is a Skyrim Souls RE related DLL.
bool IsSoulsRE(std::wstring_view dllName);

// Global flag indicating whether Skyrim Souls RE is active.
bool g_IsSoulsREActive{};
} // namespace stubs
