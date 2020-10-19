#pragma once

#include <Forms/TESBoundAnimObject.h>

#include <Components/BGSDestructibleObjectForm.h>
#include <Components/BGSSkinForm.h>
#include <Components/BGSKeywordForm.h>
#include <Components/BGSAttackDataForm.h>
#include <Components/BGSPerkRankArray.h>
#include <Components/TESActorBaseData.h>
#include <Components/TESContainer.h>
#include <Components/TESSpellList.h>
#include <Components/TESAIForm.h>
#include <Components/TESFullName.h>


struct ActorValueOwner
{
    virtual ~ActorValueOwner();

    virtual float GetCurrent(uint32_t aId);
    virtual void sub_2();
    virtual float GetBase(uint32_t aId);
    virtual void sub_4();
    virtual void sub_5();
    virtual void ForceCurrent(bool aModify, uint32_t aId, float aValue);
    virtual void sub_7();
    virtual void sub_8();
};

struct TESActorBase : TESBoundAnimObject
{
    TESActorBaseData actorData;
    TESContainer container;
    TESSpellList spellList;
    TESAIForm aiForm;
    TESFullName fullName;
    ActorValueOwner actorValueOwner;
    BGSDestructibleObjectForm destructibleObjectForm;
    BGSSkinForm skinForm;
    BGSKeywordForm keywordForm;
    BGSAttackDataForm attackDataForm;
    BGSPerkRankArray perkRanks;
};

