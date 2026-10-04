#pragma once

#include <NetImmerse/NiObject.h>
#include <NetImmerse/NiPointer.h>

struct ImageSpaceModifierInstanceForm;
struct NiAVObject;

struct ImageSpaceModifierInstance : NiObject
{
    virtual bool IsExpired();
    virtual void Apply();
    virtual void PrintInfo(char* apBuffer);
    virtual ImageSpaceModifierInstanceForm* IsForm();

    static void Stop(ImageSpaceModifierInstance* apModifier);

    float strength;
    NiPointer<NiAVObject> target;
    float age;
    uint32_t flags;
};

static_assert(offsetof(ImageSpaceModifierInstance, target) == 0x18);
static_assert(sizeof(ImageSpaceModifierInstance) == 0x28);
