#pragma once

struct TESForm;

// Fallout 4 has no left/right hand equip slots. Shared code only compares
// against these, so they stay null.
struct DefaultObjectManager
{
    static DefaultObjectManager& Get() noexcept
    {
        static DefaultObjectManager s_manager;
        return s_manager;
    }

    TESForm* leftEquipSlot{};
    TESForm* rightEquipSlot{};
};
