#include <Services/DebugService.h>
#include <imgui.h>
#include <inttypes.h>

#include <Combat/CombatController.h>
#include <Combat/CombatGroup.h>
#include <Combat/CombatTargetSelector.h>
#include <Games/Misc/BGSWorldLocation.h>
#include <Combat/CombatInventory.h>

#include <math.h>

namespace
{
struct DetectionState : public NiRefObject
{
    ~DetectionState() override;

    uint32_t level;
    uint8_t unk14;
    uint8_t unk15;
    uint8_t unk16;
    uint8_t pad17;
    float unk18;
    NiPoint3 unk1C;
    float unk28;
    NiPoint3 unk2C;
    float unk38;
    NiPoint3 unk3C;
};
static_assert(sizeof(DetectionState) == 0x48);

BGSEncounterZone* GetLocationEncounterZone(Actor* apActor)
{
    TP_THIS_FUNCTION(TGetLocationEncounterZone, BGSEncounterZone*, Actor);
    POINTER_SKYRIMSE(TGetLocationEncounterZone, getLocationEncounterZone, 20203);
    return TiltedPhoques::ThisCall(getLocationEncounterZone, apActor);
}

bool IsValidTarget(CombatTargetSelector* apThis, Actor* apAttacker, Actor* apTarget, BGSEncounterZone* apEncounterZone)
{
    TP_THIS_FUNCTION(TIsValidTarget, bool, CombatTargetSelector, Actor* apAttacker, Actor* apTarget,
                     BGSEncounterZone* apEncounterZone);
    POINTER_SKYRIMSE(TIsValidTarget, isValidTarget, 47196);
    return TiltedPhoques::ThisCall(isValidTarget, apThis, apAttacker, apTarget, apEncounterZone);
}

DetectionState* GetDetectionState(Actor* apAttacker, Actor* apTarget)
{
    TP_THIS_FUNCTION(TGetDetectionState, DetectionState*, Actor, Actor* apTarget);
    POINTER_SKYRIMSE(TGetDetectionState, getDetectionState, 37757);
    return TiltedPhoques::ThisCall(getDetectionState, apAttacker, apTarget);
}

float GetCombatTargetSelectorDetectionTimeLimit()
{
    POINTER_SKYRIMSE(float, s_value, 382393);
    return *s_value;
}

float GetCombatTargetSelectorRecentLOSTimeLimit()
{
    POINTER_SKYRIMSE(float, s_value, 382400);
    return *s_value;
}

bool ActorHasEquippedRangedWeapon_m(Actor* apActor)
{
    TP_THIS_FUNCTION(TActorHasEquippedRangedWeapon_m, bool, Actor);
    POINTER_SKYRIMSE(TActorHasEquippedRangedWeapon_m, actorHasEquippedRangedWeapon_m, 47303);
    return TiltedPhoques::ThisCall(actorHasEquippedRangedWeapon_m, apActor);
}

BGSWorldLocation* RefrGetWorldLocation(TESObjectREFR* apThis, BGSWorldLocation* apResult)
{
    TP_THIS_FUNCTION(TRefrGetWorldLocation, BGSWorldLocation*, TESObjectREFR, BGSWorldLocation*);
    POINTER_SKYRIMSE(TRefrGetWorldLocation, refrGetWorldLocation, 19784);
    return TiltedPhoques::ThisCall(refrGetWorldLocation, apThis, apResult);
}

float GetModifiedDistance(BGSWorldLocation* apThis, BGSWorldLocation* apWorldLocation)
{
    TP_THIS_FUNCTION(TGetModifiedDistance, float, BGSWorldLocation, BGSWorldLocation*);
    POINTER_SKYRIMSE(TGetModifiedDistance, getModifiedDistance, 18518);
    return TiltedPhoques::ThisCall(getModifiedDistance, apThis, apWorldLocation);
}

float GetDword_142FE5B78()
{
    POINTER_SKYRIMSE(float, s_value, 405282);
    return *s_value;
}

bool CheckMovement(CombatController* apThis, BGSWorldLocation* apWorldLocation)
{
    TP_THIS_FUNCTION(TCheckMovement, bool, CombatController, BGSWorldLocation*);
    POINTER_SKYRIMSE(TCheckMovement, checkMovement, 33261);
    return TiltedPhoques::ThisCall(checkMovement, apThis, apWorldLocation);
}

bool sub_1407E7A40(Actor* apThis, BGSWorldLocation* apWorldLocation)
{
    TP_THIS_FUNCTION(TFunc, bool, Actor, BGSWorldLocation*);
    POINTER_SKYRIMSE(TFunc, func, 47307);
    return TiltedPhoques::ThisCall(func, apThis, apWorldLocation);
}

bool IsFleeing(Actor* apThis)
{
    TP_THIS_FUNCTION(TFunc, bool, Actor);
    POINTER_SKYRIMSE(TFunc, func, 37577);
    return TiltedPhoques::ThisCall(func, apThis);
}

float CalculateTargetScore(CombatTargetSelector* apThis, CombatTarget* apCombatTarget, Actor* apAttacker, Actor* apTarget, BGSEncounterZone* apEncounterZone)
{
    float score = 0.f;

    CombatController* pCombatController = apThis->pCombatController;
    Actor* pCachedAttacker = pCombatController->pCachedAttacker;

    BGSWorldLocation pAttackerWorldLocation{};
    RefrGetWorldLocation(apAttacker, &pAttackerWorldLocation);
    BGSWorldLocation pTargetWorldLocation{};
    RefrGetWorldLocation(apTarget, &pTargetWorldLocation);

    float aiTime = AITimer::GetAITime();

    auto* pDetectionState = GetDetectionState(apAttacker, apTarget);
    if (pDetectionState && (aiTime - pDetectionState->unk38) <= GetCombatTargetSelectorDetectionTimeLimit())
    {
        if (pDetectionState->unk15)
            score = 1000.f;
        else if (pCachedAttacker == apTarget &&
                 (aiTime - pDetectionState->unk18) < GetCombatTargetSelectorRecentLOSTimeLimit())
            score = 1000.f;

        if (pDetectionState->unk14)
            score += 100.f;
    }

    if (pCachedAttacker == apTarget)
        score += 100.f;

    if (ActorHasEquippedRangedWeapon_m(apAttacker) == ActorHasEquippedRangedWeapon_m(apTarget))
        score += 100.f;

    bool isSameWorldLocation = pTargetWorldLocation.pSpace == pAttackerWorldLocation.pSpace;
    float distanceFactor = 0.f;
    if (isSameWorldLocation)
        distanceFactor = GetModifiedDistance(&pAttackerWorldLocation, &pTargetWorldLocation);
    else if (apTarget != pCachedAttacker)
        distanceFactor = 2048.f;

    score += (float)((float)(1.0 - (float)(fminf(distanceFactor, 2048.f) * 0.00048828125f)) * 1000.f);

    if (apCombatTarget->attackerCount > (pCachedAttacker == apTarget))
        score += GetDword_142FE5B78();

    if (pCombatController->pInventory->maximumRange < 1024.f || !isSameWorldLocation)
    {
        if (!CheckMovement(pCombatController, &pTargetWorldLocation))
            score -= 2000.f;
        if (!isSameWorldLocation && !sub_1407E7A40(apAttacker, &pTargetWorldLocation))
            score -= 2500.f;
    }

    if (apTarget->VirtIsDead(false))
        score -= 500.f;

    if (IsFleeing(apTarget))
        score -= 500.f;

    return score;
}
} // namespace

void DebugService::DrawCombatView()
{
    if (!m_formId)
        return;

    ImGui::Begin("Combat");

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, (void*)&m_formId, nullptr, nullptr, "%" PRIx32, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

    Actor* pActor = Cast<Actor>(TESForm::GetById(m_formId));

    if (pActor && pActor->pCombatController && pActor->pCombatController->pCombatGroup && pActor->pCombatController->pActiveTargetSelector)
    {
        Actor* pTarget = Cast<Actor>(TESObjectREFR::GetByHandle(pActor->pCombatController->targetHandle));
        if (pTarget)
        {
            uint32_t targetFormID = pTarget->formID;
            ImGui::InputScalar("Current target ID", ImGuiDataType_U32, (void*)&targetFormID, nullptr, nullptr, "%" PRIx32, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }

        BGSEncounterZone* pZone = GetLocationEncounterZone(pActor);

        ImGui::BeginChild("Potential targets", ImVec2(0, 200), true);

        auto& targets = pActor->pCombatController->pCombatGroup->targets;
        for (int i = 0; i < targets.length; i++)
        {
            auto& target = targets[i];
            Actor* pTarget = Cast<Actor>(TESObjectREFR::GetByHandle(target.targetHandle));
            if (pTarget)
            {
                const uint32_t formID = pTarget->formID;
                ImGui::InputScalar("Form ID", ImGuiDataType_U32, (void*)&formID, nullptr, nullptr, "%" PRIx32, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

                float score =
                    CalculateTargetScore(pActor->pCombatController->pActiveTargetSelector, &target, pActor, pTarget, pZone);

                ImGui::InputFloat("Score", &score, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            }

            ImGui::Separator();
        }

        ImGui::EndChild();
    }

    ImGui::End();
}
