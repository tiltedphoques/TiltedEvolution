#include <TiltedOnlinePCH.h>

#include <Games/Animation/IAnimationGraphManagerHolder.h>

#include <BSAnimationGraphManager.h>

#if !defined(TP_FALLOUT4)
bool IAnimationGraphManagerHolder::GetVariableNiPoint3(BSFixedString* apVariable, NiPoint3* apReturn)
{
    TP_THIS_FUNCTION(TGetNiPoint3Variable, bool, IAnimationGraphManagerHolder, BSFixedString*, NiPoint3*);
    POINTER_SKYRIMSE(TGetNiPoint3Variable, InternalGetNiPoint3Variable, 32884);

    return TiltedPhoques::ThisCall(InternalGetNiPoint3Variable, this, apVariable, apReturn);
}

#endif

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

#if defined(TP_FALLOUT4)
bool IAnimationGraphManagerHolder::IsReady()
{
    BSAnimationGraphManager* pAnimationGraph = nullptr;
    const auto result = GetBSAnimationGraph(&pAnimationGraph);

    if (pAnimationGraph)
        pAnimationGraph->Release();

    return result;
}

IAnimationGraphManagerHolder::AnimationGraphManagerPtr IAnimationGraphManagerHolder::GetAnimationGraphManagerPtr() const noexcept
{
    BSAnimationGraphManager* pManager = nullptr;
    GetBSAnimationGraph(&pManager);
    return {
        pManager, [](BSAnimationGraphManager* apManager)
        {
            apManager->Release();
        }};
}
#else
bool IAnimationGraphManagerHolder::IsReady()
{
    BSTSmartPointer<BSAnimationGraphManager> spManager;
    return GetBSAnimationGraph(spManager);
}

IAnimationGraphManagerHolder::AnimationGraphManagerPtr IAnimationGraphManagerHolder::GetAnimationGraphManagerPtr() const noexcept
{
    BSTSmartPointer<BSAnimationGraphManager> spManager;
    GetBSAnimationGraph(spManager);

    return spManager;
}
#endif
