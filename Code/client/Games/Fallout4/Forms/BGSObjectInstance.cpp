#include "BGSObjectInstance.h"

BGSObjectInstance::BGSObjectInstance(TESForm* apObject, TBO_InstanceData* apInstanceData)
{
    TP_THIS_FUNCTION(TBGSObjectInstance, void, BGSObjectInstance, TESForm* apObject, TBO_InstanceData* apInstanceData);
    POINTER_FALLOUT4(TBGSObjectInstance, bgsObjectInstance, 1095749);
    ThisCall(bgsObjectInstance, this, apObject, apInstanceData);
}
