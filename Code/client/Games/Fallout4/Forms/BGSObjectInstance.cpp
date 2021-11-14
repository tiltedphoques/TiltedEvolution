#include "BGSObjectInstance.h"

BGSObjectInstance::BGSObjectInstance(TESForm* apObject, TBO_InstanceData* apInstanceData)
{
    TP_THIS_FUNCTION(TBGSObjectInstance, void, BGSObjectInstance, TESForm* apObject, TBO_InstanceData* apInstanceData);

    POINTER_FALLOUT4(TBGSObjectInstance, bgsObjectInstance, 0x1402F7B50 - 0x140000000);

    ThisCall(bgsObjectInstance, this, apObject, apInstanceData);
}
