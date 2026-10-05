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

    void* parent;          // 28
    float localRotate[12]; // 30
    NiPoint3 localTranslate; // 60
    float localScale;      // 6C
    float worldRotate[12]; // 70
    NiPoint3 worldTranslate; // A0
    float worldScale;      // AC
    NiPoint3 boundCenter;  // B0
    float boundRadius;     // BC
    uint8_t padC0[0x108 - 0xC0];
    uint64_t flags;        // 108
    uint64_t userData;     // 110
    float fadeAmount;      // 118
    uint8_t pad11C[0x120 - 0x11C];
};

static_assert(offsetof(NiAVObject, worldTranslate) == 0xA0);
static_assert(offsetof(NiAVObject, flags) == 0x108);

static_assert(sizeof(NiAVObject) == 0x120);
