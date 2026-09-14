#pragma once

#include <Games/Skyrim/Forms/TESObjectCELL.h>

struct NiPoint3;

namespace CellRespawnOverrides
{
bool GetRespawnPos(const char* apCellEditorId, NiPoint3& aOutPos) noexcept;
bool GetRespawnPos(TESObjectCELL* apCell, NiPoint3& aOutPos) noexcept;
}
