#pragma once

#include <NetImmerse/NiObjectNET.h>
#include <Games/Primitives.h>

struct BSFixedString;

struct NiAVObject : NiObjectNET
{
    virtual ~NiAVObject();

    virtual void sub_26();
    virtual void sub_27();
    virtual void sub_28();
    virtual void sub_29();
    virtual NiAVObject* GetByName(BSFixedString& aName);

    uint8_t pad30[0xA0 - 0x30];
    // Translation of the world transform (which starts at 0x7C).
    NiPoint3 worldTranslate;
    uint8_t padAC[0x110 - 0xAC];
};

static_assert(offsetof(NiAVObject, worldTranslate) == 0xA0);
static_assert(sizeof(NiAVObject) == 0x110);
