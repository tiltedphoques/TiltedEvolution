#pragma once

#if defined(TP_FALLOUT4)
#include <memory>
#else
#include <BSCore/BSTSmartPointer.h>
#endif

struct BSAnimationGraphManager;
struct BSFixedString;
struct NiPoint3;

struct IAnimationGraphManagerHolder
{
    virtual ~IAnimationGraphManagerHolder();

    virtual bool SendAnimationEvent(BSFixedString* apAnimEvent);

#if defined(TP_FALLOUT4)
    virtual void EventSuccessfullyProcessed();
    virtual void ModifyInitialAnimationState(bool aInitialState);
    virtual bool GetBSAnimationGraph(BSAnimationGraphManager** aPtr) const;
    virtual uint32_t sub_5();
    virtual uint32_t sub_6();
    virtual uint32_t sub_7();
    virtual uint32_t sub_8();
    virtual uint32_t sub_9();
    virtual uint32_t sub_A();
    virtual uint32_t sub_B();
    virtual uint32_t sub_C();
    virtual uint32_t sub_D();
    virtual uint32_t sub_E();
    virtual uint32_t sub_F();
    virtual bool GetGraphVariableByIdFloat(uint32_t aId, float& aValue) const;
    virtual bool GetGraphVariableByIdBool(uint32_t aId, bool& aValue) const;
    virtual bool GetGraphVariableByIdInt(uint32_t aId, int32_t& aValue) const;
#else
    virtual bool GetBSAnimationGraph(BSTSmartPointer<BSAnimationGraphManager>& aOut) const;
    virtual uint32_t sub_3();
    virtual uint32_t sub_4();
    virtual uint32_t sub_5();
    virtual uint32_t sub_6();
    virtual uint32_t sub_7();
    virtual uint32_t sub_8();
    virtual uint32_t sub_9();
    virtual uint32_t sub_A();
    virtual uint32_t sub_B();
    virtual uint32_t sub_C();
    virtual uint32_t sub_D();
    virtual uint32_t sub_E();
    virtual uint32_t sub_F();
#endif

    virtual bool GetVariableFloat(BSFixedString* apVariable, float* apReturn);
    virtual bool GetVariableInt(BSFixedString* apVariable, uint32_t* apReturn);
    virtual bool GetVariableBool(BSFixedString* apVariable, bool* apReturn);
#if defined(TP_FALLOUT4)
    virtual void PreUpdateAnimationGraphManager();
    virtual void PostUpdateAnimationGraphManager();
    virtual void PreLoadAnimationGraphManager();
    virtual void PostLoadAnimationGraphManager();
#endif

#if !defined(TP_FALLOUT4)
    bool GetVariableNiPoint3(BSFixedString* apVariable, NiPoint3* apReturn);
#endif
    bool SetVariableFloat(BSFixedString* apVariable, float aValue);
    bool SetVariableInt(BSFixedString* apVariable, int32_t aValue);
    bool SetVariableBool(BSFixedString* apVariable, bool aValue);
    bool IsReady();
    bool RevertAnimationGraphManager();
#if defined(TP_FALLOUT4)
    using AnimationGraphManagerPtr = std::unique_ptr<BSAnimationGraphManager, void (*)(BSAnimationGraphManager*)>;
#else
    using AnimationGraphManagerPtr = BSTSmartPointer<BSAnimationGraphManager>;
#endif
    AnimationGraphManagerPtr GetAnimationGraphManagerPtr() const noexcept;
};
