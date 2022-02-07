#include "BGSPerk.h"

#include <Actor.h>
#include <Games/Misc/TaskQueueInterface.h>
#include <AI/AIProcess.h>

extern uint32_t s_nextPerkFormId;

// TODO: this is BGSPerk::ApplyPerksVisitor::operator()
TP_THIS_FUNCTION(TAddPerk, int64_t, BGSPerk::ApplyPerksVisitor, BGSPerk::PerkRankData* apPerkData);
static TAddPerk* RealAddPerk = nullptr;

int64_t TP_MAKE_THISCALL(HookAddPerk, BGSPerk::ApplyPerksVisitor, BGSPerk::PerkRankData* apPerkData)
{
    if (apPerkData && apPerkData->pPerk)
    {
        int8_t oldRank = 0;
        const int8_t newRank = apPerkData->cCurrentRank;
        const uint32_t formId = apThis->pActor->formID;

        if (formId == s_nextPerkFormId)
        {
            s_nextPerkFormId = 0;
            if (formId != 0x14)
                oldRank |= 0x100;
        }

        TaskQueueInterface::Get()->QueueApplyPerk(apThis->pActor, apPerkData->pPerk, oldRank, newRank);
    }

    return 1;
}

TP_THIS_FUNCTION(TApplyPerk, void, BGSPerk, Actor* apActor, const uint32_t acOldRank, const uint8_t acNewRank);
static TApplyPerk* RealApplyPerk = nullptr;

void TP_MAKE_THISCALL(HookApplyPerk, BGSPerk, Actor* apActor, const uint32_t acOldRank, const uint8_t acNewRank)
{
    if ((acOldRank & 0x100) != 0)
    {
        AIProcess* pProcess = apActor->currentProcess;
        if (pProcess)
            pProcess->ClearPerks();
    }

    ThisCall(RealApplyPerk, apThis, apActor, acOldRank, acNewRank);
}

static TiltedPhoques::Initializer s_perkHooks([]() 
{
    POINTER_SKYRIMSE(TAddPerk, s_addPerk, 0x14034EEA0 - 0x140000000);
    POINTER_SKYRIMSE(TApplyPerk, s_applyPerk, 0x14034EA30 - 0x140000000);

    RealAddPerk = s_addPerk.Get();
    RealApplyPerk = s_applyPerk.Get();

    TP_HOOK(&RealAddPerk, HookAddPerk);
    TP_HOOK(&RealApplyPerk, HookApplyPerk);


    // TODO: probably a better way to do this

    // replace byte movzx with 4 byte mov right before calling BGSPerk::ApplyPerk() in taskqueue unpac
    uint8_t mov1[] = { 0x44, 0x8B, 0x47, 0x18, 0x90 };
    void* destination1 = (void*)0x1405ECD14;
    memcpy(destination1, mov1, 5);

    // TaskQueueInterface::QueueApplyPerk(), movzx   edi, r9b -> mov edi, r9d
    uint8_t mov2[] = { 0x44, 0x89, 0xCF, 0x90 };
    void* destination2 = (void*)0x1405EA76A;
    memcpy(destination2, mov2, 4);

    // TaskQueueInterface::QueueApplyPerk(), movzx   eax, dil -> mov eax, edi
    uint8_t mov3[] = { 0x89, 0xF8, 0x90, 0x90 };
    void* destination3 = (void*)0x1405EA796;
    memcpy(destination3, mov3, 4);
});
