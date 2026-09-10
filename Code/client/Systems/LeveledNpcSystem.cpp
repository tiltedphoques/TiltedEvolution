#include <TiltedOnlinePCH.h>

#include <Systems/LeveledNpcSystem.h>
#include <Components.h>
#include <World.h>
#include <Games/References.h>
#include <Forms/TESNPC.h>

void LeveledNpcSystem::Initialize(entt::registry& aRegistry) noexcept
{
    aRegistry.on_construct<LeveledNpcConformComponent>().connect<&LeveledNpcSystem::OnConformStarted>();
    aRegistry.on_destroy<LeveledNpcConformComponent>().connect<&LeveledNpcSystem::OnConformFinished>();
}

void LeveledNpcSystem::OnConformStarted(entt::registry& aRegistry, const entt::entity aEntity) noexcept
{
    const auto& conform = aRegistry.get<LeveledNpcConformComponent>(aEntity);
    auto* pActor = Cast<Actor>(TESForm::GetById(conform.ActorFormId));
    if (pActor && pActor == conform.ExpectedActor)
    {
        pActor->GetExtension()->SetReenabling(true);
        pActor->GetExtension()->GraphDescriptorHash = 0;
    }
}

void LeveledNpcSystem::OnConformFinished(entt::registry& aRegistry, const entt::entity aEntity) noexcept
{
    const auto& conform = aRegistry.get<LeveledNpcConformComponent>(aEntity);
    auto* pActor = Cast<Actor>(TESForm::GetById(conform.ActorFormId));
    if (pActor && pActor == conform.ExpectedActor)
    {
        pActor->GetExtension()->GraphDescriptorHash = 0;
        pActor->GetExtension()->SetReenabling(false);
    }
}

TESNPC* LeveledNpcSystem::ResolveCustomSpawnNpcBase(World& aWorld, const GameId& acPickId, const GameId& acBaseId) noexcept
{
    const auto resolveNpc = [&aWorld](const GameId& acId, const char* apKind) -> TESNPC*
    {
        const uint32_t cFormId = aWorld.GetModSystem().GetGameId(acId);
        if (cFormId == 0)
        {
            spdlog::warn("{} NPC {:X}:{:X} is not resolvable, possibly because a mod is missing", apKind, acId.ModId, acId.BaseId);
            return nullptr;
        }

        TESForm* pForm = TESForm::GetById(cFormId);
        TESNPC* pNpc = Cast<TESNPC>(pForm);
        if (!pNpc || pNpc->IsTemporary())
        {
            spdlog::warn("{} form {:X}:{:X} resolved to {:X}, which is not a static NPC base", apKind, acId.ModId, acId.BaseId, cFormId);
            return nullptr;
        }

        return pNpc;
    };

    if (acPickId != GameId{})
    {
        if (TESNPC* pPick = resolveNpc(acPickId, "Leveled pick"))
            return pPick;

        spdlog::warn("Falling back to the placed NPC base after failing to resolve the leveled pick");
    }
    else
    {
        spdlog::warn("No leveled NPC pick was provided; falling back to the placed NPC base");
    }

    if (acBaseId != GameId{})
    {
        if (TESNPC* pBase = resolveNpc(acBaseId, "Placed base"))
            return pBase;
    }
    else
    {
        spdlog::warn("No placed NPC base was provided for the custom spawn fallback");
    }

    spdlog::error("Neither the leveled pick nor the placed base resolves to a valid NPC; aborting custom spawn");
    return nullptr;
}

// A static base that still uses a leveled-character template is an unresolved
// placed shell. It has no usable model until the canonical pick is applied.
bool LeveledNpcSystem::IsUnresolvedShell(const TESNPC* apBase) noexcept
{
    if (!apBase || apBase->IsTemporary())
        return false;

    const TESForm* pTemplate = apBase->actorData.baseTemplateForm;
    return pTemplate && pTemplate->formType == FormType::LeveledCharacter;
}
