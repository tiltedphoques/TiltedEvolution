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
#include <Misc/ActorValueOwner.h>

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

