#include <TiltedOnlinePCH.h>

#include <Games/Animation/IAnimationGraphManagerHolder.h>

#include <BSAnimationGraphManager.h>

bool IAnimationGraphManagerHolder::SetVariableFloat(BSFixedString* apVariable, float aValue)
{
    TP_THIS_FUNCTION(TSetFloatVariable, bool, IAnimationGraphManagerHolder, BSFixedString*, float);
    POINTER_GAME(TSetFloatVariable, InternalSetFloatVariable, 32887, 2214545);

    return TiltedPhoques::ThisCall(InternalSetFloatVariable, this, apVariable, aValue);
}

bool IAnimationGraphManagerHolder::SetVariableInt(BSFixedString* apVariable, int32_t aValue)
{
    TP_THIS_FUNCTION(TSetIntVariable, bool, IAnimationGraphManagerHolder, BSFixedString*, int32_t);
    POINTER_GAME(TSetIntVariable, InternalSetIntVariable, 32886, 2214544);

    return TiltedPhoques::ThisCall(InternalSetIntVariable, this, apVariable, aValue);
}

bool IAnimationGraphManagerHolder::SetVariableBool(BSFixedString* apVariable, bool aValue)
{
    TP_THIS_FUNCTION(TSetBoolVariable, bool, IAnimationGraphManagerHolder, BSFixedString*, bool);
    POINTER_GAME(TSetBoolVariable, InternalSetBoolVariable, 32885, 2214543);

    return TiltedPhoques::ThisCall(InternalSetBoolVariable, this, apVariable, aValue);
}

bool IAnimationGraphManagerHolder::RevertAnimationGraphManager()
{
    #if defined(TP_FALLOUT4)
    TP_THIS_FUNCTION(TRevertAnimationGraphManager, bool, IAnimationGraphManagerHolder, bool);
#else
    TP_THIS_FUNCTION(TRevertAnimationGraphManager, bool, IAnimationGraphManagerHolder);
#endif
    POINTER_GAME(TRevertAnimationGraphManager, InternalRevertAnimationGraphManager, 32883, 2214541);

    #if defined(TP_FALLOUT4)
    return TiltedPhoques::ThisCall(InternalRevertAnimationGraphManager, this, true);
#else
    return TiltedPhoques::ThisCall(InternalRevertAnimationGraphManager, this);
#endif
}

bool IAnimationGraphManagerHolder::IsReady()
{
    BSAnimationGraphManager* pAnimationGraph = nullptr;
    const auto result = GetBSAnimationGraph(&pAnimationGraph);

    if (pAnimationGraph)
        pAnimationGraph->Release();

    return result;
}
