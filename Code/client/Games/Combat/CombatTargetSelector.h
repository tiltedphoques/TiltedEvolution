#pragma once

// TODO: ft

struct CombatTargetSelector
{
    virtual ~CombatTargetSelector();
    virtual void Unk1();
    virtual void Unk2();
    virtual void Unk3();
    virtual void Unk4();
    virtual void Update();
    virtual BSPointerHandle<Actor> SelectTarget(); // TODO: is RVO implemented here? Probably.

    void *vftable_NiRefObject_8;
    uint8_t unk10[0x10];
    uint32_t flags;
    uint8_t unk24[4];
};
