#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESBoundAnimObject.h>

#include <Games/Skyrim/Components/BGSDestructibleObjectForm.h>
#include <Games/Skyrim/Components/BGSSkinForm.h>
#include <Games/Skyrim/Components/BGSKeywordForm.h>
#include <Games/Skyrim/Components/BGSAttackDataForm.h>
#include <Games/Skyrim/Components/BGSPerkRankArray.h>
#include <Games/Skyrim/Components/TESActorBaseData.h>
#include <Games/Skyrim/Components/TESContainer.h>
#include <Games/Skyrim/Components/TESSpellList.h>
#include <Games/Skyrim/Components/TESAIForm.h>
#include <Games/Skyrim/Components/TESFullName.h>


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

#endif
