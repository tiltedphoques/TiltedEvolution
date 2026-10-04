#pragma once

#include <Components/BaseFormComponent.h>
#include <Misc/BSFixedString.h>

enum class FormType : uint8_t
{
    Armor = 0x1D,
    Book = 0x1E,
    Container = 0x1F,
    Door = 0x20,
    Ingredient = 0x21,
    Weapon = 0x2B,
    Ammo = 0x2C,
    Npc = 0x2D,
    LeveledCharacter = 0x2E,
    Alchemy = 0x30,
    LeveledItem = 0x38,
    Character = 0x41,
    QuestItem = 0x50,
    Count = 0x9F
};

struct BGSSaveFormBuffer;
struct BGSLoadFormBuffer;

struct TESForm : BaseFormComponent
{
    struct ChangeFlags
    {
        uint32_t flags{};
        uint64_t unk4{};
        uint32_t unkC{};
    };

    enum FormFlags
    {
        DELETED = 1 << 5,
        DISABLED = 1 << 0xB,
        IGNORE_FRIENDLY_HITS = 1 << 0x14,
    };

    static TESForm* GetById(uint32_t aId);

    virtual void sub_7();
    virtual void sub_8();
    virtual void sub_9();
    virtual void sub_A();
    virtual void sub_B();
    virtual void sub_C();
    virtual bool MarkChanged(uint32_t aChangeFlag);
    virtual void UnsetChanged(uint32_t aChangeFlag);
    virtual void sub_F();
    virtual void sub_10();
    virtual void Save(BGSSaveFormBuffer* apBuffer) const noexcept;
    virtual void Load(BGSLoadFormBuffer* apBuffer);
    virtual void sub_13();
    virtual void sub_14();
    virtual void sub_15();
    virtual void InitializeComponents();
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
    virtual const char* GetName(const BSFixedString& acTag = {}) const noexcept;
    virtual void CopyFrom(TESForm* apForm);
    virtual void sub_37();
    virtual void sub_38();
    virtual void sub_39();
    virtual const char* GetFormEditorID();
    virtual void sub_3B();
    virtual void sub_3C();
    virtual void sub_3D();
    virtual void sub_3E();
    virtual void sub_3F();
    virtual void ActivateReference();
    virtual void sub_41();
    virtual void sub_42();
    virtual void sub_43();
    virtual void sub_44();
    virtual void sub_45();
    virtual void sub_46();
    virtual void sub_47();
    virtual void sub_48();
    virtual void sub_49();

    // void CopyFromEx(TESForm* rhs);
    void Save_Reversed(uint32_t aChangeFlags, Buffer::Writer& aWriter);
    void SetSkipSaveFlag(bool aSet) noexcept;
    uint32_t GetChangeFlags() const noexcept;

    bool GetIgnoreFriendlyHit() const noexcept { return (flags & IGNORE_FRIENDLY_HITS) != 0; }
    void SetIgnoreFriendlyHit(bool aSet) noexcept
    {
        if (aSet)
            flags |= IGNORE_FRIENDLY_HITS;
        else
            flags &= ~IGNORE_FRIENDLY_HITS;
    }

    bool IsDisabled() const noexcept { return (flags & DISABLED) != 0; }
    bool IsDeleted() const noexcept { return (flags & DELETED) != 0; }
    bool IsTemporary() const noexcept { return formID >= 0xFF000000; }
    bool IsConsumable() const noexcept { return formType == FormType::Ingredient || formType == FormType::Alchemy; }

    uintptr_t unk4;
    uint32_t flags;
    uint32_t formID;
    uint16_t unk10;
    FormType formType;
    uint8_t padForm;
};

static_assert(sizeof(TESForm) == 0x20);
