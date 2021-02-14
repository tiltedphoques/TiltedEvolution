#pragma once

#include <Effects/ActiveEffect.h>

struct Actor;
struct ActorValueInfo;

struct ValueModifierEffect : public ActiveEffect
{
    virtual void Function25();
    virtual void Function26();
    virtual void Function27();
    virtual void Function28();
    virtual void Function29();
    virtual void Function30();
    virtual void Function31();
    virtual void ApplyActorEffect(Actor* actor, float effectValue, unsigned int unk1);

    char pad_0080[16];
    uint32_t actorValueIndex;
    char pad_0094[4];
    ActorValueInfo* actorValueInfo;
    
};
static_assert(sizeof(ValueModifierEffect) == 0xA0);
