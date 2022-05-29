#include <TiltedOnlinePCH.h>

#include <Games/Overrides.h>

thread_local uint32_t ScopedEquipOverride::s_refCount = 0;
thread_local uint32_t ScopedUnequipOverride::s_refCount = 0;
thread_local uint32_t ScopedSaveLoadOverride::s_refCount = 0;
thread_local uint32_t ScopedExperienceOverride::s_refCount = 0;
thread_local uint32_t ScopedActivateOverride::s_refCount = 0;
thread_local uint32_t ScopedInventoryOverride::s_refCount = 0;
thread_local uint32_t ScopedExtraDataOverride::s_refCount = 0;
thread_local uint32_t ScopedQuestOverride::s_refCount = 0;
thread_local uint32_t ScopedSpellCastOverride::s_refCount = 0;
