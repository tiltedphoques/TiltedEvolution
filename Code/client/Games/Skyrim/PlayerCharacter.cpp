#include <TiltedOnlinePCH.h>

#include <PlayerCharacter.h>

#include <Events/InventoryChangeEvent.h>

#include <World.h>

TP_THIS_FUNCTION(TPickUpItem, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);

static TPickUpItem* RealPickUpItem = nullptr;

char TP_MAKE_THISCALL(HookPickUpItem, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealPickUpItem, apThis, apObject, aCount, aUnk1, aUnk2);
}

static TiltedPhoques::Initializer s_playerCharacterHooks([]()
    {
        POINTER_SKYRIMSE(TPickUpItem, s_pickUpItem, 0x1406CD770 - 0x140000000);

        RealPickUpItem = s_pickUpItem.Get();

        TP_HOOK(&RealPickUpItem, HookPickUpItem);
    });
