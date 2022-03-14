#include <TiltedOnlinePCH.h>

#include <PlayerCharacter.h>

#include <Events/InventoryChangeEvent.h>

#include <World.h>

TP_THIS_FUNCTION(TPickUpItem, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);

static TPickUpItem* RealPickUpItem = nullptr;

extern thread_local bool g_modifyingInventory;

char TP_MAKE_THISCALL(HookPickUpItem, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    if (!g_modifyingInventory)
    {
        auto& modSystem = World::Get().GetModSystem();

        Inventory::Entry item{};
        modSystem.GetServerModId(apObject->formID, item.BaseId);
        item.Count = aCount;

        // TODO: not sure about this
        if (apObject->GetExtraDataList())
            apThis->GetItemFromExtraData(item, apObject->GetExtraDataList());

        World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item)));
    }

    return ThisCall(RealPickUpItem, apThis, apObject, aCount, aUnk1, aUnk2);
}

static TiltedPhoques::Initializer s_playerCharacterHooks([]()
{
    POINTER_SKYRIMSE(TPickUpItem, s_pickUpItem, 40533);

    RealPickUpItem = s_pickUpItem.Get();

    TP_HOOK(&RealPickUpItem, HookPickUpItem);
});
