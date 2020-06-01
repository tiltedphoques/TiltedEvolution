#include <Games/Animation/IAnimationGraphManagerHolder.h>

#include <Games/Skyrim/BSAnimationGraphManager.h>
#include <Games/Fallout4/BSAnimationGraphManager.h>

bool IAnimationGraphManagerHolder::SetVariableFloat(BSFixedString* apVariable, float aValue)
{
    TP_THIS_FUNCTION(TSetFloatVariable, bool, IAnimationGraphManagerHolder, BSFixedString*, float);

    POINTER_SKYRIMSE(TSetFloatVariable, InternalSetFloatVariable, 0x1404F0720 - 0x140000000);
    POINTER_FALLOUT4(TSetFloatVariable, InternalSetFloatVariable, 0x14081D410 - 0x140000000);

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
