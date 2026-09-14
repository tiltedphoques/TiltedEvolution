#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>

struct TESObjectREFR;
struct BGSListForm;
struct BGSOutfit;

struct TESReactionForm : BaseFormComponent
{
    void* reactionListHead; // BSSimpleList<GROUP_REACTION*>
    void* reactionListNext;
    uint8_t groupFormType;
    uint8_t pad19[7];
};

static_assert(sizeof(TESReactionForm) == 0x20);

struct TESFaction : TESForm
{
    // Where the game sends the player's belongings when they are arrested by this faction.
    struct CrimeData
    {
        TESObjectREFR* jailMarker;               // JAIL
        TESObjectREFR* waitMarker;               // WAIT
        TESObjectREFR* stolenGoodsContainer;     // STOL: stolen items taken on arrest
        TESObjectREFR* playerInventoryContainer; // PLCN: the rest of the player's inventory, returned on release
        BGSListForm* crimeGroup;                 // CRGR
        BGSOutfit* jailOutfit;                   // JOUT
        uint8_t crimeValues[0x18];               // CRVA
    };

    TESFullName fullname;
    TESReactionForm reactionForm;
    uint8_t crimeGoldMap[0x30]; // BSTHashMap<const TESNPC*, uint32_t>
    uint32_t factionFlags;      // DATA
    uint32_t pad84;
    CrimeData crimeData;
};

static_assert(offsetof(TESFaction, fullname) == 0x20);
static_assert(offsetof(TESFaction, crimeData) == 0x88);
static_assert(offsetof(TESFaction, crimeData.stolenGoodsContainer) == 0x98);
static_assert(offsetof(TESFaction, crimeData.playerInventoryContainer) == 0xA0);
