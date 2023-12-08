#include "Actor.h"

#include <Managers/TESDataHandler.h>
#include <Memory.h>
#include <Forms/FormExtensions.h>
#include <Forms/TESObjectCELL.h>

BSTSmartPointer<Actor> Actor::New() noexcept
{
    const auto pActor = Memory::Allocate<Actor>();

    using TActorCtor = Actor*(Actor*, int32_t* a2, uint32_t aAddToProcessList, uint32_t a4);
    TActorCtor* actorCtor = (TActorCtor*)0x142453970; // 150180
    int32_t a2 = 0; // Seems to be unused?
    actorCtor(pActor, &a2, true, 0);

    return pActor;
}

BSTSmartPointer<Actor> Actor::Create(TESForm* apBaseForm) noexcept
{
    auto pActor = New();
    // Prevent saving
    pActor->SetSkipSaveFlag(true);
    pActor->GetExtension()->SetRemote(true);

    const auto pPlayer = PlayerCharacter::Get();
    auto pCell = pPlayer->pParentCell;
    const auto pWorldSpace = pPlayer->GetWorldSpace();

    //pActor->SetLevelMod(4); // TODO: ?
    pActor->AddChange(0x40000000);
    pActor->SetParentCell(pCell);
    //pActor->SetBaseForm(apBaseForm); // Function seems to have disappeared?
    pActor->pBaseForm = apBaseForm;

    auto position = pPlayer->position;
    auto rotation = pPlayer->rotation;

    if (pCell && !pCell->IsInteriorCell())
        pCell = nullptr;

    TESDataHandler::Get()->CreateReferenceAtLocation(position, rotation, pCell, pWorldSpace, pActor);

    //pActor->ForcePosition(position, true);

    // TODO: still correct?
    pActor->formFlags &= 0xFFDFFFFF;

    return pActor;
}

// TODO: is this the best hook? Damage issues in STR.
// TODO: hook this
bool Actor::DoDamage(double aDamage, Actor* apSource, bool aKillMove) noexcept
{
    using TDoDamage = bool(Actor*, double, Actor*, bool);
    TDoDamage* doDamage = (TDoDamage*)0x14247C8E4; // 150573
    return doDamage(this, aDamage, apSource, aKillMove);
}

// TODO: hook this
float Actor::SpeakSound(const char* apFile) noexcept
{
    /*
    if (apThis->GetExtension()->IsLocal())
        World::Get().GetRunner().Trigger(DialogueEvent(apThis->formID, apName));
    */

    using TSpeakSound = float(Actor*, const char* apFile, uint64_t* apHandle, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7);
    TSpeakSound* speakSound = (TSpeakSound*)0x1424B12F8; // 151348

    uint64_t handle{};
    return speakSound(this, apFile, &handle, 0, 0, 0, 0, 0);
}

bool Actor::Kill() noexcept
{
    using TKill = bool(Actor*, TESObjectREFR* apKiller, double aDamage, bool aSendEvent, bool aRagdollInstant, bool a5, bool a6);
    TKill* kill = (TKill*)0x142497464; // 151028
    return kill(this, nullptr, 0.0, true, false, true, true);
}
