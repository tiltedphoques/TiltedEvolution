#pragma once

struct ImageSpaceModifierInstanceForm;
struct NiAVObject;

struct ImageSpaceModifierInstance : public NiObject
{
    ~ImageSpaceModifierInstance() override;

    virtual bool Unk25(void);
    virtual void Apply() = 0;
    virtual ImageSpaceModifierInstanceForm* IsForm();
    virtual void Unk28(void*) = 0;

    uint32_t unk;
    float strength;
    NiPointer<NiAVObject> target;
    float age;
    uint32_t flags;

    static void Stop(ImageSpaceModifierInstance* apMod)
    {
        using TClearImageSpaceModifier = void(ImageSpaceModifierInstance*);
        POINTER_SKYRIMSE(TClearImageSpaceModifier, s_clearImageSpaceModifier, 18563);
        return s_clearImageSpaceModifier.Get()(apMod);
    }
};

static_assert(offsetof(ImageSpaceModifierInstance, target) == 0x18);
static_assert(sizeof(ImageSpaceModifierInstance) == 0x28);
