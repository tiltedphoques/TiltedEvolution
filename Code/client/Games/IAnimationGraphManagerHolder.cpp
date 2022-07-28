#include <TiltedOnlinePCH.h>

#include <Games/Animation/IAnimationGraphManagerHolder.h>

#include <BSAnimationGraphManager.h>

bool IAnimationGraphManagerHolder::SetVariableFloat(BSFixedString* apVariable, float aValue)
{
    TP_THIS_FUNCTION(TSetFloatVariable, bool, IAnimationGraphManagerHolder, BSFixedString*, float);

    POINTER_SKYRIMSE(TSetFloatVariable, InternalSetFloatVariable, 32887);
    POINTER_FALLOUT4(TSetFloatVariable, InternalSetFloatVariable, 27401);

    return ThisCall(InternalSetFloatVariable, this, apVariable, aValue);
}

bool IAnimationGraphManagerHolder::IsReady()
{
    BSAnimationGraphManager* pAnimationGraph = nullptr;
    const auto result = GetBSAnimationGraph(&pAnimationGraph);

    if (pAnimationGraph)
        pAnimationGraph->Release();

    return result;
}
