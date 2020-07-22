#if TP_FALLOUT4

#include <Games/References.h>
#include <Games/Memory.h>
#include <Games/Overrides.h>

void TESObjectREFR::SaveInventory(BGSSaveFormBuffer* apBuffer) const noexcept
{
    TP_THIS_FUNCTION(TSaveFunc, void, void, BGSSaveFormBuffer*);

    POINTER_FALLOUT4(TSaveFunc, s_save, 0x1401ACB20 - 0x140000000);

    ThisCall(s_save, inventory, apBuffer);
}

void TESObjectREFR::LoadInventory(BGSLoadFormBuffer* apBuffer) noexcept
{
    TP_THIS_FUNCTION(TLoadFunc, void, void, BGSLoadFormBuffer*);

    POINTER_FALLOUT4(TLoadFunc, s_load, 0x1401ACC00 - 0x140000000);

    ThisCall(s_load, inventory, apBuffer);
}

void TESObjectREFR::RemoveAllItems() noexcept
{
    using TRemoveAllItems = void(void*, void*, TESObjectREFR*, TESObjectREFR*, bool);

    POINTER_FALLOUT4(TRemoveAllItems, s_removeAllItems, 0x14140CE90 - 0x140000000);

    s_removeAllItems(nullptr, nullptr, this, nullptr, false);
}


#endif
