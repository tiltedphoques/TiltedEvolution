#pragma once

#include <Effects/ActiveEffect.h>

struct Actor;
struct ActorValueInfo;

struct ValueModifierEffect : public ActiveEffect
{
    virtual void sub_19();
    virtual void sub_1A();
    virtual void sub_1B();
    virtual void sub_1C();
    virtual void sub_1D();
    virtual void sub_1E();
    virtual void sub_1F();
    virtual void ApplyActorEffect(Actor* actor, float effectValue, unsigned int unk1);

    char pad_0080[16];
    uint32_t actorValueIndex;
    char pad_0094[4];
    ActorValueInfo* actorValueInfo;
    
};
static_assert(sizeof(ValueModifierEffect) == 0xA0);
