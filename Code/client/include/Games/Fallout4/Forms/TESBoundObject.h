#pragma once

#include <Forms/TESObject.h>

struct TESBoundObject : TESObject
{
    virtual void sub_54();
    virtual void sub_55();
    virtual void sub_56();
    virtual void sub_57();
    virtual void sub_58();
    virtual void sub_59();
    virtual void sub_5A();
    virtual void sub_5B();
    virtual void sub_5C();
    virtual void sub_5D();
    virtual void sub_5E();
    virtual void sub_5F();
    virtual void sub_60();
    virtual void sub_61();
    virtual void sub_62();
    virtual void sub_63();
    virtual void sub_64();

    uint8_t pad20[0x68 - 0x20];
};

static_assert(sizeof(TESBoundObject) == 0x68);
