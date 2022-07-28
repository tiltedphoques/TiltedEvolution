#pragma once

#include <Components/BaseFormComponent.h>

enum class FormType : uint8_t
{
    Armor = 29,
    Book = 30,
    Container = 31,
    Door = 32,
    Ammo = 44,
    Npc = 45,
    LeveledCharacter = 46,
    Character = 65,
    QuestItem = 80,
    Count = 0x9F
};

struct BGSSaveFormBuffer;
struct BGSLoadFormBuffer;

struct TESForm : BaseFormComponent
{
    struct ChangeFlags
    {
        uint32_t flags;
        uint64_t unk4;
        uint32_t unkC;
    };

    enum FormFlags
    {
        DISABLED = 1 << 0xB,
    };

    static TESForm* GetById(uint32_t aId);

    virtual void sub_7();
    virtual void sub_8();
    virtual void sub_9();
    virtual void sub_A();
    virtual void sub_B();
    virtual void sub_C();
    virtual bool MarkChanged(uint32_t aChangeFlag);
    virtual bool UnsetChanged(uint32_t aChangeFlag);
    virtual void sub_F();
    virtual void sub_10();
    virtual bool Save(BGSSaveFormBuffer* apBuffer) const noexcept;
    virtual bool Load(BGSLoadFormBuffer* apBuffer);
    virtual void sub_13();
    virtual void sub_14();
    virtual void sub_15();
    virtual void sub_16();
    virtual void sub_17();
    virtual void sub_18();
    virtual FormType GetFormType();
    virtual void sub_1A();
    virtual void sub_1B();
    virtual void sub_1C();
    virtual void sub_1D();
    virtual void sub_1E();
    virtual void sub_1F();
    virtual void sub_20();
    virtual void sub_21();
    virtual void sub_22();
    virtual void sub_23();
    virtual void sub_24();
    virtual void sub_25();
    virtual void sub_26();
    virtual void sub_27();
    virtual void sub_28();
    virtual void sub_29();
    virtual void sub_2A();
    virtual void sub_2B();
    virtual void sub_2C();
    virtual void sub_2D();
    virtual void sub_2E();
    virtual void sub_2F();
    virtual void sub_30();
    virtual void sub_31();
    virtual void sub_32();
    virtual void sub_33();
    virtual void sub_34();
    virtual const char* GetName() const noexcept;
    virtual void sub_36();
    virtual void sub_37();
    virtual void sub_38();
    virtual void sub_39();
    virtual void sub_3A();
    virtual void sub_3B();
    virtual void sub_3C();
    virtual void sub_3D();
    virtual void sub_3E();
    virtual void sub_3F();
    virtual void sub_40();
    virtual void sub_41();
    virtual void sub_42();
    virtual void sub_43();
    virtual void sub_44();
    virtual void sub_45();
    virtual void sub_46();
    virtual void sub_47();

    //void CopyFromEx(TESForm* rhs);
    void Save(std::string apSaveBuffer) noexcept;
    void Save_Reversed(uint32_t aChangeFlags, Buffer::Writer& aWriter);
    void SetSkipSaveFlag(bool aSet) noexcept;
    uint32_t GetChangeFlags() const noexcept;

    void SetIgnoreFriendlyHit(bool aSet) noexcept 
    {
        // TODO: fallout 4 impl
        return;
    }
    bool IsDisabled() const noexcept { return (flags & DISABLED) != 0; }
    bool IsTemporary() const noexcept { return formID >= 0xFF000000; }

    uintptr_t unk8;
    uint32_t flags;
    uint32_t formID;
    uint16_t unk10;
    FormType formType;
    uint8_t unk1B;
    uint8_t tp_flags; // This is actually padding so we can use it for any purpose
};

static_assert(sizeof(TESForm) == 0x20);
