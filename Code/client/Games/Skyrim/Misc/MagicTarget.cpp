#include "MagicTarget.h"

#include <Actor.h>
#include <Games/ActorExtension.h>

TP_THIS_FUNCTION(TAddTarget, bool, MagicTarget, MagicTarget::AddTargetData& arData);
TP_THIS_FUNCTION(TCheckAddEffect, bool, MagicTarget::AddTargetData, void* arArgs, float afResistance);

static TAddTarget* RealAddTarget = nullptr;
static TCheckAddEffect* RealCheckAddEffect = nullptr;

static thread_local bool s_autoSucceedEffectCheck = false;

bool MagicTarget::AddTarget(AddTargetData& arData) noexcept
{
    s_autoSucceedEffectCheck = true;
    bool result = ThisCall(RealAddTarget, this, arData);
    s_autoSucceedEffectCheck = false;
    return result;
}

bool TP_MAKE_THISCALL(HookAddTarget, MagicTarget, MagicTarget::AddTargetData& arData)
{
    if (arData.pCaster)
    {
        if (auto pExtension = arData.pCaster->GetExtension())
        {
            if (pExtension->IsRemote())
                return false;
        }
    }

    return ThisCall(RealAddTarget, apThis, arData);
}

bool TP_MAKE_THISCALL(HookCheckAddEffect, MagicTarget::AddTargetData, void* arArgs, float afResistance)
{
    if (s_autoSucceedEffectCheck)
        return true;

    return ThisCall(RealCheckAddEffect, apThis, arArgs, afResistance);
}

static TiltedPhoques::Initializer s_magicTargetHooks([]() {
    POINTER_SKYRIMSE(TAddTarget, addTarget, 0x140633090 - 0x140000000);
    POINTER_SKYRIMSE(TCheckAddEffect, checkAddEffect, 0x1405535C0 - 0x140000000);

    RealAddTarget = addTarget.Get();
    RealCheckAddEffect = checkAddEffect.Get();

    TP_HOOK(&RealAddTarget, HookAddTarget);
    TP_HOOK(&RealCheckAddEffect, HookCheckAddEffect);
});

