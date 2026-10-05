#include <TiltedOnlinePCH.h>

#include <BSAnimationGraphManager.h>
#include <Havok/BShkbAnimationGraph.h>
#include <Havok/BShkbHkxDB.h>
#include <Havok/hkbBehaviorGraph.h>

#include <Structs/AnimationVariables.h>

#include <TiltedCore/Hash.hpp>

namespace
{
// Behavior variables replicated between clients, matched by name so the first
// person graph of a player maps onto the third person graph of their puppet.
// Variables a graph does not have are skipped.
constexpr const char* kSyncedBooleans[] = {
    "IsSprinting", "isJumping", "bInJumpState", "bInLandingState", "bAimActive", "bAimEnabled", "isSightedOver", "isFiring", "isReloading",
    "IsAttackReady", "isAttackNotReady", "bEquipOk", "bIsThrowing", "IsBlocking", "IsSneaking", "bIsSneaking", "IsStaggering"};

constexpr const char* kSyncedIntegers[] = {
    "iSyncIdleLocomotion", "iSyncSprintState", "iSyncTurnState", "iSyncRunDirection", "iSyncLocomotionSpeed", "iSyncWalkRun", "iSyncForwardBackward",
    "iLocomotionSpeedState", "iSyncSneakWalkRun", "iSyncDirection", "iSyncShuffleState", "iSyncJumpState", "CurrentJumpState", "iSyncSightedState",
    "iAttackState", "iMeleeState", "iSyncFireState", "iSyncReadyAlertRelaxed", "iSyncGunDown", "iRifleDrawnStateID", "RifleDrawnCurrentState",
    "iWantBlock", "iIsInSneak", "iGetUpType", "Pose"};

constexpr const char* kSyncedFloats[] = {
    "Speed", "Direction", "TurnDelta", "SpeedSmoothed", "DirectionSmoothed", "TurnDeltaSmoothed", "TurnDeltaDamped", "DirectionDamped",
    "speedDamped", "SampledSpeed", "VelocityZ", "fSpeedWalk", "fSpeedRun", "DirectionDegrees", "Pitch", "PitchDelta", "PitchOffset",
    "PitchDeltaSmoothed", "AimHeadingCurrent", "AimPitchCurrent", "weaponSpeedMult", "ReloadSpeedMult", "staggerMagnitude", "staggerDirection"};

struct GraphVariable
{
    uint32_t Index;
    hkbVariableInfo::Type Type;
};

TiltedPhoques::Map<String, GraphVariable> GetGraphVariables(const BShkbAnimationGraph* apGraph)
{
    TiltedPhoques::Map<String, GraphVariable> variables;

    const auto* pDb = apGraph->hkxDB;
    const auto* pBehavior = apGraph->behaviorGraph;
    if (!pDb || !pBehavior || !pBehavior->data || !pBehavior->animationVariables)
        return variables;

    const auto& map = pDb->animationVariables;
    if (!map.entries)
        return variables;

    for (uint32_t i = 0; i < map.capacity; ++i)
    {
        const auto& entry = map.entries[i];
        if (!entry.next || entry.index < 0 || entry.index >= pBehavior->data->variableInfoCount)
            continue;
        if (static_cast<uint32_t>(entry.index) >= pBehavior->animationVariables->size)
            continue;

        variables[entry.name.AsAscii()] = {static_cast<uint32_t>(entry.index), pBehavior->data->variableInfos[entry.index].type};
    }

    return variables;
}

const BShkbAnimationGraph* GetActiveGraph(BSAnimationGraphManager* apManager, int aForceIndex = -1)
{
    const uint32_t index = aForceIndex == -1 ? apManager->animationGraphIndex : static_cast<uint32_t>(aForceIndex);
    if (index >= apManager->animationGraphs.size)
        return nullptr;
    return apManager->animationGraphs.Get(index);
}

// Graph indices of the synced variables, -1 where the graph lacks the variable or its type differs.
struct SyncedIndices
{
    Vector<int32_t> Booleans;
    Vector<int32_t> Integers;
    Vector<int32_t> Floats;
};

template <size_t N>
Vector<int32_t> ResolveIndices(const TiltedPhoques::Map<String, GraphVariable>& acVariables, const char* const (&acNames)[N],
                               std::initializer_list<hkbVariableInfo::Type> aTypes)
{
    Vector<int32_t> indices(N, -1);
    for (size_t i = 0; i < N; ++i)
    {
        const auto it = acVariables.find(acNames[i]);
        if (it != acVariables.end() && std::find(aTypes.begin(), aTypes.end(), it->second.Type) != aTypes.end())
            indices[i] = static_cast<int32_t>(it->second.Index);
    }
    return indices;
}

// Projects are shared between actors, so the lookup is done once per project.
const SyncedIndices* GetSyncedIndices(const BShkbAnimationGraph* apGraph)
{
    static TiltedPhoques::Map<const BShkbHkxDB*, SyncedIndices> s_cache;

    if (!apGraph->hkxDB)
        return nullptr;

    const auto it = s_cache.find(apGraph->hkxDB);
    if (it != s_cache.end())
        return &it->second;

    const auto variables = GetGraphVariables(apGraph);
    if (variables.empty())
        return nullptr;

    SyncedIndices indices;
    indices.Booleans = ResolveIndices(variables, kSyncedBooleans, {hkbVariableInfo::kBool});
    indices.Integers = ResolveIndices(variables, kSyncedIntegers, {hkbVariableInfo::kInt8, hkbVariableInfo::kInt16, hkbVariableInfo::kInt32});
    indices.Floats = ResolveIndices(variables, kSyncedFloats, {hkbVariableInfo::kReal});
    return &(s_cache[apGraph->hkxDB] = std::move(indices));
}

// The active graph is the first person one while a player is in first person.
bool ResolveGraph(BSAnimationGraphManager* apManager, const SyncedIndices*& apIndices, hkbVariableValueSet<uint32_t>*& apVariables)
{
    const auto* pGraph = GetActiveGraph(apManager);
    if (!pGraph || !pGraph->behaviorGraph || !pGraph->behaviorGraph->animationVariables)
        return false;

    apIndices = GetSyncedIndices(pGraph);
    apVariables = pGraph->behaviorGraph->animationVariables;
    return apIndices != nullptr;
}
} // namespace

SortedMap<uint32_t, String> BSAnimationGraphManager::DumpAnimationVariables(bool aPrintVariables)
{
    SortedMap<uint32_t, String> variables;

    if (const auto* pGraph = GetActiveGraph(this))
    {
        for (const auto& [name, variable] : GetGraphVariables(pGraph))
            variables[variable.Index] = name;
    }

    if (aPrintVariables)
    {
        for (auto& [id, name] : variables)
            spdlog::info("k{} = {},", name, id);
    }

    return variables;
}

uint64_t BSAnimationGraphManager::GetDescriptorKey(int aForceIndex)
{
    String variableNames{};

    if (const auto* pGraph = GetActiveGraph(this, aForceIndex))
    {
        std::map<uint32_t, String> ordered;
        for (const auto& [name, variable] : GetGraphVariables(pGraph))
            ordered[variable.Index] = name;

        for (auto& [index, name] : ordered)
            variableNames += name;
    }

    std::transform(variableNames.begin(), variableNames.end(), variableNames.begin(), [](unsigned char c) { return std::tolower(c); });

    return TiltedPhoques::FHash::Crc64(reinterpret_cast<const unsigned char*>(variableNames.c_str()), variableNames.size());
}

void TESObjectREFR::SaveAnimationVariables(AnimationVariables& aVariables) const noexcept
{
    BSAnimationGraphManager* pManager = nullptr;
    if (!animationGraphHolder.GetBSAnimationGraph(&pManager) || !pManager)
        return;

    aVariables.Booleans.assign(std::size(kSyncedBooleans), false);
    aVariables.Integers.assign(std::size(kSyncedIntegers), 0);
    aVariables.Floats.assign(std::size(kSyncedFloats), 0.f);

    const SyncedIndices* pIndices = nullptr;
    hkbVariableValueSet<uint32_t>* pVariables = nullptr;
    if (ResolveGraph(pManager, pIndices, pVariables))
    {
        const auto size = static_cast<int32_t>(pVariables->size);

        for (size_t i = 0; i < pIndices->Booleans.size(); ++i)
        {
            const auto idx = pIndices->Booleans[i];
            if (idx >= 0 && idx < size)
                aVariables.Booleans[i] = pVariables->data[idx] != 0;
        }

        for (size_t i = 0; i < pIndices->Integers.size(); ++i)
        {
            const auto idx = pIndices->Integers[i];
            if (idx >= 0 && idx < size)
                aVariables.Integers[i] = pVariables->data[idx];
        }

        for (size_t i = 0; i < pIndices->Floats.size(); ++i)
        {
            const auto idx = pIndices->Floats[i];
            if (idx >= 0 && idx < size)
                aVariables.Floats[i] = *reinterpret_cast<const float*>(&pVariables->data[idx]);
        }
    }

    pManager->Release();
}

void TESObjectREFR::LoadAnimationVariables(const AnimationVariables& aVariables) const noexcept
{
    BSAnimationGraphManager* pManager = nullptr;
    if (!animationGraphHolder.GetBSAnimationGraph(&pManager) || !pManager)
        return;

    const SyncedIndices* pIndices = nullptr;
    hkbVariableValueSet<uint32_t>* pVariables = nullptr;
    if (ResolveGraph(pManager, pIndices, pVariables))
    {
        const auto size = static_cast<int32_t>(pVariables->size);

        for (size_t i = 0; i < pIndices->Booleans.size() && i < aVariables.Booleans.size(); ++i)
        {
            const auto idx = pIndices->Booleans[i];
            if (idx >= 0 && idx < size)
                pVariables->data[idx] = aVariables.Booleans[i];
        }

        for (size_t i = 0; i < pIndices->Integers.size() && i < aVariables.Integers.size(); ++i)
        {
            const auto idx = pIndices->Integers[i];
            if (idx >= 0 && idx < size)
                pVariables->data[idx] = aVariables.Integers[i];
        }

        for (size_t i = 0; i < pIndices->Floats.size() && i < aVariables.Floats.size(); ++i)
        {
            const auto idx = pIndices->Floats[i];
            if (idx >= 0 && idx < size)
                *reinterpret_cast<float*>(&pVariables->data[idx]) = aVariables.Floats[i];
        }
    }

    pManager->Release();
}
