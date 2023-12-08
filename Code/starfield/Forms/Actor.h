#pragma once

#include "TESObjectREFR.h"
#include <BSCore/BSTSmartPointer.h>

struct ActorExtension;
struct ExActor;
struct ExPlayerCharacter;

struct Actor : TESObjectREFR
{
    static constexpr FormType Type = FormType::ACHR;
    // Allocs and calls constructor
    static BSTSmartPointer<Actor> New() noexcept;
    // Places a brand new actor in the world
    static BSTSmartPointer<Actor> Create(TESForm* apBaseForm) noexcept;

    // Casting
    ActorExtension* GetExtension() noexcept;
    ExActor* AsExActor() noexcept;
    ExPlayerCharacter* AsExPlayerCharacter() noexcept;

    bool DoDamage(double aDamage, Actor* apSource, bool aKillMove) noexcept;
    float SpeakSound(const char* apFile) noexcept;
    bool Kill() noexcept;

    virtual void sub_130();
    virtual void sub_131();
    virtual void sub_132();
    virtual void sub_133();
    virtual void sub_134();
    virtual void sub_135();
    virtual void sub_136();
    virtual void SetPosition(const NiPoint3& acoint, bool aSyncHavok);
    virtual void sub_138();
    virtual void Resurrect(bool aResetInventory, bool aAttach3D); // Think about which parameters to use
    virtual void sub_13A();
    virtual void sub_13B();
    virtual void sub_13C();
    virtual void sub_13D();
    virtual void sub_13E();
    virtual void sub_13F();
    virtual void sub_140();
    virtual void sub_141();
    virtual void sub_142();
    virtual void sub_143();
    virtual void sub_144();
    virtual void sub_145();
    virtual void sub_146();
    virtual void sub_147();
    virtual void sub_148();
    virtual void sub_149();
    virtual void sub_14A();
    virtual void sub_14B();
    virtual void sub_14C();
    virtual void sub_14D();
    virtual void sub_14E();
    virtual void sub_14F();
    virtual void sub_150();
    virtual void sub_151();
    virtual void sub_152();
    virtual void sub_153();
    virtual void sub_154();
    virtual void sub_155();
    virtual void sub_156();
    virtual void sub_157();
    virtual void sub_158();
    virtual void sub_159();
    virtual void sub_15A();
    virtual void sub_15B();
    virtual void sub_15C();
    virtual void sub_15D();
    virtual void sub_15E();
    virtual void sub_15F();
    virtual void sub_160();
    virtual void sub_161();
    virtual void sub_162();
    virtual void sub_163();
    virtual void sub_164();
    virtual void sub_165();
    virtual void sub_166();
    virtual void sub_167();
    virtual void sub_168();
    virtual void sub_169();
    virtual void sub_16A();
    virtual void sub_16B();
    virtual void sub_16C();
    virtual void sub_16D();
    virtual void sub_16E();
    virtual void sub_16F();
    virtual void sub_170();
    virtual void sub_171();
    virtual void sub_172();
    virtual void sub_173();
    virtual void sub_174();
    virtual void sub_175();
    virtual void sub_176();
    virtual void sub_177();
    virtual void sub_178();
    virtual void sub_179();
    virtual void sub_17A();
    virtual void sub_17B();
    virtual void sub_17C();
    virtual void sub_17D();
    virtual void sub_17E();
    virtual void sub_17F();
    virtual void sub_180();
    virtual void sub_181();
    virtual void sub_182();
    virtual void sub_183();
    virtual void sub_184();
    virtual void sub_185();
    virtual void sub_186();
    virtual void sub_187();
    virtual void sub_188();
    virtual void sub_189();
    virtual void sub_18A();
    virtual void sub_18B();
    virtual void sub_18C();
    virtual void sub_18D();
    virtual void sub_18E();
    virtual void sub_18F();
    virtual void sub_190();
    virtual void sub_191();
    virtual void sub_192();
    virtual void sub_193();
    virtual void sub_194();
    virtual void sub_195();
    virtual void sub_196();
    virtual void sub_197();
    virtual void sub_198();
    virtual void sub_199();
    virtual void sub_19A();
    virtual void sub_19B();
    virtual void sub_19C();
    virtual void sub_19D();
    virtual void sub_19E();
    virtual void sub_19F();
    virtual void sub_1A0();
    virtual void sub_1A1();

    uint8_t unkActor[0x260 - sizeof(TESObjectREFR)];
    void* pCurrentProcess;
    uint8_t unk268[0x550 - 0x268];
};

static_assert(sizeof(Actor) == 0x550);