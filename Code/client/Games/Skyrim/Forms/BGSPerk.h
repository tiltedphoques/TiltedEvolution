#pragma once

#include "TESForm.h"
#include <Components/TESFullName.h>
#include <Components/TESDescription.h>
#include <Components/TESIcon.h>

struct Actor;

struct BGSPerk : TESForm, TESFullName, TESDescription, TESIcon
{
    struct PerkRankData
    {
        BGSPerk* pPerk;
        // TODO: unsigned?
        int8_t cCurrentRank;
    };

    struct ApplyPerksVisitor
    {
        virtual void Exec();

        Actor* pActor;
    };
};
