#pragma once

#include <Games/Fallout4/Common.h>
#include <Games/Skyrim/Common.h>

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


#if TP_PLATFORM_64
static_assert(offsetof(internal::_TestChannel, data) == 0x18);
static_assert(offsetof(internal::_TestChannel, reference) == 0x28);
#else
static_assert(offsetof(internal::_TestChannel, data) == 0xC);
static_assert(offsetof(internal::_TestChannel, reference) == 0x14);
#endif

