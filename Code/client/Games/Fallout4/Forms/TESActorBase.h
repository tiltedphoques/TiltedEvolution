#pragma once

#include <Components/BGSPropertySheet.h>
#include <Components/BGSPerkRankArray.h>
#include <Components/BGSAttackDataForm.h>
#include <Components/TESAIForm.h>
#include <Components/TESSpellList.h>
#include <Components/TESContainer.h>
#include <Components/BGSDestructibleObjectForm.h>
#include <Components/TESActorBaseData.h>
#include <Components/BGSSkinForm.h>
#include <Components/BGSKeywordForm.h>
#include <Components/TESFullName.h>

#include <Misc/ActorValueOwner.h>
#include <Forms/TESBoundAnimObject.h>

struct TESActorBase : TESBoundAnimObject
{
    virtual void sub_65();
    virtual void sub_66();
    virtual void sub_67();
    virtual void sub_68();

    TESActorBaseData actorData; // 68
    TESContainer container; // D0
    TESSpellList spellList; // E8
    TESAIForm aiForm; // F8
    TESFullName	fullName; // 120
    ActorValueOwner actorValueOwner; // 130
    BGSDestructibleObjectForm destructibleObjectForm; // 138
    BGSSkinForm skinForm; // 148
    BGSKeywordForm keywordForm; // 158
    BGSAttackDataForm attackDataForm; // 178
    BGSPerkRankArray perkRankArray; // 188
    BGSPropertySheet propertySheet; // 1A0
};

static_assert(offsetof(TESActorBase, actorData) == 0x68);
static_assert(offsetof(TESActorBase, container) == 0xD0);
static_assert(offsetof(TESActorBase, spellList) == 0xE8);
static_assert(offsetof(TESActorBase, aiForm) == 0xF8);
static_assert(offsetof(TESActorBase, fullName) == 0x120);
static_assert(offsetof(TESActorBase, actorValueOwner) == 0x130);
static_assert(offsetof(TESActorBase, destructibleObjectForm) == 0x138);
static_assert(offsetof(TESActorBase, skinForm) == 0x148);
static_assert(offsetof(TESActorBase, propertySheet) == 0x1A0);
static_assert(sizeof(TESActorBase) == 0x1B0);

