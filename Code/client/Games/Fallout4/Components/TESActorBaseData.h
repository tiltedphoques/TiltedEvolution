#pragma once

#include <Components/BaseFormComponent.h>

struct BGSVoiceType;

struct TESActorBaseData : BaseFormComponent
{
    virtual ~TESActorBaseData();
    virtual void sub_7();
    virtual void sub_8();
    virtual void sub_9();

    struct alignas(sizeof(void*)) FactionInfo
    {
        struct TESFaction* faction;
        int8_t rank;
    };

    enum BaseFlags
    {
        IS_ESSENTIAL = 1 << 1,
    };

    // TODO: ft
    // idk if this is correct
    bool IsEssential() const noexcept
    {
        return flags & BaseFlags::IS_ESSENTIAL;
    }
    void SetEssential(bool aSet) noexcept
    {
        if (aSet)
            flags |= BaseFlags::IS_ESSENTIAL;
        else
            flags &= ~BaseFlags::IS_ESSENTIAL;
    }

    uint64_t flags;
    uint8_t unk10[0x28 - 0x10];
    BGSVoiceType* voiceType;
    TESForm* owner;
    uint8_t unk30[0x50 - 0x38];
    GameArray<FactionInfo> factions;
};

static_assert(sizeof(TESActorBaseData) == 0x68);
static_assert(offsetof(TESActorBaseData, owner) == 0x30);
static_assert(offsetof(TESActorBaseData, factions) == 0x50);
