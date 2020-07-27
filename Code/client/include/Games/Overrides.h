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
    struct SaveLoad {};
}

using ScopedEquipOverride = ScopedOverride<details::Equip>;
using ScopedSaveLoadOverride = ScopedOverride<details::SaveLoad>;
