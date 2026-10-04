#pragma once

#include <NetImmerse/NiObjectNET.h>

struct BSFixedString;

struct NiAVObject : NiObjectNET
{
    virtual ~NiAVObject();

    virtual void sub_26();
    virtual void sub_27();
    virtual void sub_28();
    virtual void sub_29();
    virtual NiAVObject* GetByName(BSFixedString& aName);

    uint8_t pad30[0x110 - 0x30];
};

static_assert(sizeof(NiAVObject) == 0x110);
