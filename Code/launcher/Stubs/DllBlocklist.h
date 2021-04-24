#pragma once

#include <string>

namespace stubs
{
    // Returns true if the module name is found in the hard block list.
    bool IsDllBlocked(std::wstring_view dllName);
}
