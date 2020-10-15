
#if TP_FALLOUT4

#include <Games/Fallout4/Forms/TESNPC.h>

TP_THIS_FUNCTION(TSetLeveledNpc, TESNPC*, TESNPC, TESNPC*);
static TSetLeveledNpc* RealSetLeveledNpc = nullptr;

TESNPC* TP_MAKE_THISCALL(HookSetLeveledNpc, TESNPC, TESNPC* apSelectedNpc)
{
    spdlog::info("For TESNPC: {}, spawning: {}", apThis->fullName.value.AsAscii(), apSelectedNpc->fullName.value.AsAscii());

    return ThisCall(RealSetLeveledNpc, apThis, RTTI_CAST(TESForm::GetById(0x3B547), TESForm, TESNPC));
}


static TiltedPhoques::Initializer s_npcInitHooks([]() {
    POINTER_FALLOUT4(TSetLeveledNpc, s_SetLeveledNpc, 0x140149340 - 0x140000000);

    RealSetLeveledNpc = s_SetLeveledNpc.Get();

    //TP_HOOK(&RealSetLeveledNpc, HookSetLeveledNpc);
});


#endif
