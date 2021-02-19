#include <stdafx.h>

#include <Forms/TESNPC.h>

TP_THIS_FUNCTION(TSetLeveledNpc, TESNPC*, TESNPC, TESNPC*);
static TSetLeveledNpc* RealSetLeveledNpc = nullptr;

TESNPC* TP_MAKE_THISCALL(HookSetLeveledNpc, TESNPC, TESNPC* apSelectedNpc)
{
    spdlog::info("For TESNPC: {}, spawning: {}", apThis->fullName.value, apSelectedNpc->fullName.value);

    return ThisCall(RealSetLeveledNpc, apThis, RTTI_CAST(TESForm::GetById(0x3B547), TESForm, TESNPC));
}


static TiltedPhoques::Initializer s_npcInitHooks([]() {
    POINTER_SKYRIMSE(TSetLeveledNpc, s_SetLeveledNpc, 0x14018C390 - 0x140000000);

    RealSetLeveledNpc = s_SetLeveledNpc.Get();

    //TP_HOOK(&RealSetLeveledNpc, HookSetLeveledNpc);
});
