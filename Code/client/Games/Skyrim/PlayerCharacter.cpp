#include <TiltedOnlinePCH.h>

#include <PlayerCharacter.h>
#include <Games/ActorExtension.h>

#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>

#include <Events/InventoryChangeEvent.h>
#include <Events/LeaveBeastFormEvent.h>

#include <World.h>

TP_THIS_FUNCTION(TPickUpItem, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);
TP_THIS_FUNCTION(TSetBeastForm, void, void, void* apUnk1, void* apUnk2, bool aEntering);

static TPickUpItem* RealPickUpItem = nullptr;
static TSetBeastForm* RealSetBeastForm = nullptr;

char TP_MAKE_THISCALL(HookPickUpItem, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealPickUpItem, apThis, apObject, aCount, aUnk1, aUnk2);
}

void TP_MAKE_THISCALL(HookSetBeastForm, void, void* apUnk1, void* apUnk2, bool aEntering)
{
    if (!aEntering)
    {
        PlayerCharacter::Get()->GetExtension()->GraphDescriptorHash = AnimationGraphDescriptor_Master_Behavior::m_key;
        World::Get().GetRunner().Trigger(LeaveBeastFormEvent());
    }

    ThisCall(RealSetBeastForm, apThis, apUnk1, apUnk2, aEntering);
}

static TiltedPhoques::Initializer s_playerCharacterHooks([]()
{
    POINTER_SKYRIMSE(TPickUpItem, s_pickUpItem, 40533);
    POINTER_SKYRIMSE(TSetBeastForm, s_setBeastForm, 55497);

    RealPickUpItem = s_pickUpItem.Get();
    RealSetBeastForm = s_setBeastForm.Get();

    TP_HOOK(&RealPickUpItem, HookPickUpItem);
    TP_HOOK(&RealSetBeastForm, HookSetBeastForm);
});
