#pragma once

template<class T>
struct ScopedOverride
{
    ScopedOverride()
    {
        s_refCount++;
    }

    ~ScopedOverride()
    {
        s_refCount--;
    }

    TP_NOCOPYMOVE(ScopedOverride);

    static bool IsOverriden()
    {
        return s_refCount > 0;
    }

private:

    static thread_local uint32_t s_refCount;
};

namespace details
{
    struct Equip {};
    struct Unequip {};
    struct SaveLoad {};
    struct Experience {};
    struct Activate {};
    struct Inventory {};
}

using ScopedEquipOverride = ScopedOverride<details::Equip>;
using ScopedUnequipOverride = ScopedOverride<details::Unequip>;
using ScopedSaveLoadOverride = ScopedOverride<details::SaveLoad>;
using ScopedExperienceOverride = ScopedOverride<details::Experience>;
using ScopedActivateOverride = ScopedOverride<details::Activate>;
using ScopedInventoryOverride = ScopedOverride<details::Inventory>;
