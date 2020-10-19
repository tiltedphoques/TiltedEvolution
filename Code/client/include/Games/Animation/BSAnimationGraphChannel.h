#pragma once

#include <Misc/BSFixedString.h>

struct Actor;

struct BSAnimationGraphChannel : BSIntrusiveRefCounted
{
    virtual ~BSAnimationGraphChannel();

    virtual void Update();
    virtual void sub_2();

    BSFixedString str;
};

template<class T, class U>
struct ActorDirectionChannel
{
    T data;
    void* unk;
    U* reference;
};

template<class TReference, class TType, template<class T, class U> class TChannel>
struct BSTAnimationGraphDataChannel
    : BSAnimationGraphChannel, TChannel<TType, TReference>
{
    virtual ~BSTAnimationGraphDataChannel();
};

namespace internal
{
    using _TestChannel = BSTAnimationGraphDataChannel<Actor, float, ActorDirectionChannel>;
}


static_assert(offsetof(internal::_TestChannel, data) == 0x18);
static_assert(offsetof(internal::_TestChannel, reference) == 0x28);
