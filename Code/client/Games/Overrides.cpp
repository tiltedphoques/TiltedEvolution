#include <stdafx.h>

#include <Games/Overrides.h>

thread_local uint32_t ScopedEquipOverride::s_refCount = 0;
thread_local uint32_t ScopedSaveLoadOverride::s_refCount = 0;
