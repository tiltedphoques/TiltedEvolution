#include <Services/CombatService.h>
#include <Services/TransportService.h>

#include <Events/UpdateEvent.h>
#include <Events/ProjectileLaunchedEvent.h>
#include <Events/HitEvent.h>

#include <Messages/ProjectileLaunchRequest.h>
#include <Messages/NotifyProjectileLaunch.h>

#include <Systems/ModSystem.h>
#include <World.h>

#include <Projectiles/Projectile.h>
#include <Forms/TESObjectWEAP.h>
#include <Forms/TESAmmo.h>
#include <Games/ActorExtension.h>

CombatService::CombatService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&CombatService::OnUpdate>(this);
    m_hitConnection = aDispatcher.sink<HitEvent>().connect<&CombatService::OnHitEvent>(this);
    m_localComponentRemoved = m_world.on_destroy<LocalComponent>().connect<&CombatService::OnLocalComponentRemoved>(this);
    m_projectileLaunchedConnection = aDispatcher.sink<ProjectileLaunchedEvent>().connect<&CombatService::OnProjectileLaunchedEvent>(this);
    m_projectileLaunchConnection = aDispatcher.sink<NotifyProjectileLaunch>().connect<&CombatService::OnNotifyProjectileLaunch>(this);
}

void CombatService::OnUpdate(const UpdateEvent& acEvent) const noexcept
{
    RunTargetUpdates(static_cast<float>(acEvent.Delta));
}

void CombatService::OnLocalComponentRemoved(entt::registry& aRegistry, entt::entity aEntity) const noexcept
{
    m_world.remove<CombatComponent>(aEntity);
}

void CombatService::OnProjectileLaunchedEvent(const ProjectileLaunchedEvent& acEvent) const noexcept
{
    ModSystem& modSystem = m_world.Get().GetModSystem();

    uint32_t shooterFormId = acEvent.ShooterID;
    auto view = m_world.view<FormIdComponent, LocalComponent>();
    const auto shooterEntityIt = std::find_if(std::begin(view), std::end(view), [shooterFormId, view](entt::entity entity) { return view.get<FormIdComponent>(entity).Id == shooterFormId; });

    if (shooterEntityIt == std::end(view))
        return;

    LocalComponent& localComponent = view.get<LocalComponent>(*shooterEntityIt);

    ProjectileLaunchRequest request{};

    request.OriginX = acEvent.Origin.x;
    request.OriginY = acEvent.Origin.y;
    request.OriginZ = acEvent.Origin.z;

    modSystem.GetServerModId(acEvent.ProjectileBaseID, request.ProjectileBaseID);
    modSystem.GetServerModId(acEvent.WeaponID, request.WeaponID);
    modSystem.GetServerModId(acEvent.AmmoID, request.AmmoID);

    request.ShooterID = localComponent.Id;

    request.ZAngle = acEvent.ZAngle;
    request.XAngle = acEvent.XAngle;
    request.YAngle = acEvent.YAngle;

    modSystem.GetServerModId(acEvent.ParentCellID, request.ParentCellID);
    modSystem.GetServerModId(acEvent.SpellID, request.SpellID);

    request.CastingSource = acEvent.CastingSource;

    request.Area = acEvent.Area;
    request.Power = acEvent.Power;
    request.Scale = acEvent.Scale;

    request.AlwaysHit = acEvent.AlwaysHit;
    request.NoDamageOutsideCombat = acEvent.NoDamageOutsideCombat;
    request.AutoAim = acEvent.AutoAim;
    request.DeferInitialization = acEvent.DeferInitialization;
    request.ForceConeOfFire = acEvent.ForceConeOfFire;

#if TP_SKYRIM64
    request.UnkBool1 = acEvent.UnkBool1;
    request.UnkBool2 = acEvent.UnkBool2;
#else
    request.ConeOfFireRadiusMult = acEvent.ConeOfFireRadiusMult;
    request.Tracer = acEvent.Tracer;
    request.IntentionalMiss = acEvent.IntentionalMiss;
    request.Allow3D = acEvent.Allow3D;
    request.Penetrates = acEvent.Penetrates;
    request.IgnoreNearCollisions = acEvent.IgnoreNearCollisions;
#endif

    m_transport.Send(request);
}

void CombatService::OnNotifyProjectileLaunch(const NotifyProjectileLaunch& acMessage) const noexcept
{
    ModSystem& modSystem = World::Get().GetModSystem();

    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.ShooterID](auto entity) { return remoteView.get<RemoteComponent>(entity).Id == Id; });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Shooter with remote id {:X} not found.", acMessage.ShooterID);
        return;
    }

    FormIdComponent formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);

#if TP_SKYRIM64
    Projectile::LaunchData launchData{};
#else
    ProjectileLaunchData launchData{};
#endif

    launchData.pShooter = Cast<TESObjectREFR>(TESForm::GetById(formIdComponent.Id));

    launchData.Origin.x = acMessage.OriginX;
    launchData.Origin.y = acMessage.OriginY;
    launchData.Origin.z = acMessage.OriginZ;

    const uint32_t cProjectileBaseId = modSystem.GetGameId(acMessage.ProjectileBaseID);
    launchData.pProjectileBase = TESForm::GetById(cProjectileBaseId);

#if TP_SKYRIM64
    const uint32_t cFromWeaponId = modSystem.GetGameId(acMessage.WeaponID);
    launchData.pFromWeapon = Cast<TESObjectWEAP>(TESForm::GetById(cFromWeaponId));
#endif

#if TP_FALLOUT4
    Actor* pShooter = Cast<Actor>(launchData.pShooter);
    pShooter->GetCurrentWeapon(&launchData.FromWeapon, 0);
#endif

    const uint32_t cFromAmmoId = modSystem.GetGameId(acMessage.AmmoID);
    launchData.pFromAmmo = Cast<TESAmmo>(TESForm::GetById(cFromAmmoId));

    launchData.fZAngle = acMessage.ZAngle;
    launchData.fXAngle = acMessage.XAngle;
    launchData.fYAngle = acMessage.YAngle;

    const uint32_t cParentCellId = modSystem.GetGameId(acMessage.ParentCellID);
    launchData.pParentCell = Cast<TESObjectCELL>(TESForm::GetById(cParentCellId));

    const uint32_t cSpellId = modSystem.GetGameId(acMessage.SpellID);
    launchData.pSpell = Cast<MagicItem>(TESForm::GetById(cSpellId));

    launchData.eCastingSource = (MagicSystem::CastingSource)acMessage.CastingSource;

    launchData.iArea = acMessage.Area;
    launchData.fPower = acMessage.Power;
    launchData.fScale = acMessage.Scale;

    launchData.bAlwaysHit = acMessage.AlwaysHit;
    launchData.bNoDamageOutsideCombat = acMessage.NoDamageOutsideCombat;
    launchData.bAutoAim = acMessage.AutoAim;

    launchData.bForceConeOfFire = acMessage.ForceConeOfFire;

    // always use origin, or it'll recalculate it and it desyncs
    launchData.bUseOrigin = true;

#if TP_SKYRIM64
    launchData.bUnkBool1 = acMessage.UnkBool1;
    launchData.bUnkBool2 = acMessage.UnkBool2;
#else
    launchData.eTargetLimb = -1;

    launchData.fConeOfFireRadiusMult = acMessage.ConeOfFireRadiusMult;
    launchData.bTracer = acMessage.Tracer;
    launchData.bIntentionalMiss = acMessage.IntentionalMiss;
    launchData.bAllow3D = acMessage.Allow3D;
    launchData.bPenetrates = acMessage.Penetrates;
    launchData.bIgnoreNearCollisions = acMessage.IgnoreNearCollisions;
#endif

    BSPointerHandle<Projectile> result;

    Projectile::Launch(&result, launchData);
}

void CombatService::OnHitEvent(const HitEvent& acEvent) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    // The targeting system does not apply to players, and only local actors should be considered.
    auto* pHittee = Cast<Actor>(TESForm::GetById(acEvent.HitteeId));
    if (!pHittee || pHittee->GetExtension()->IsPlayer() || pHittee->GetExtension()->IsRemote())
        return;

    // NPCs should not affect targeting.
    auto* pHitter = Cast<Actor>(TESForm::GetById(acEvent.HitterId));
    if (!pHitter || !pHitter->GetExtension()->IsPlayer())
        return;

    // If the target is not in combat, don't start combat, let the game take care of that first.
    if (!pHittee->IsInCombat())
        return;

    auto view = m_world.view<FormIdComponent, LocalComponent>(entt::exclude<ObjectComponent>);

    const auto hitteeIt = std::find_if(std::begin(view), std::end(view), [id = acEvent.HitteeId, view](entt::entity entity) { return view.get<FormIdComponent>(entity).Id == id; });

    if (hitteeIt == std::end(view))
    {
        spdlog::warn(__FUNCTION__ ": hittee form id component not found, form id: {:X}", acEvent.HitterId);
        return;
    }

    if (m_world.any_of<CombatComponent>(*hitteeIt))
        return;

    m_world.emplace_or_replace<CombatComponent>(*hitteeIt, acEvent.HitterId);

    pHittee->SetCombatTargetEx(pHitter);
}

void CombatService::RunTargetUpdates(const float acDelta) const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenUpdates = 20ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    const auto view = m_world.view<FormIdComponent, CombatComponent>();

    Vector<entt::entity> toRemove{};

    for (const auto entity : view)
    {
        auto& combatComponent = view.get<CombatComponent>(entity);
        combatComponent.Timer = combatComponent.Timer - acDelta;

        if (combatComponent.Timer <= 0.f)
        {
            toRemove.push_back(entity);
            continue;
        }

        auto* pTarget = Cast<Actor>(TESForm::GetById(combatComponent.TargetFormId));
        if (!pTarget)
        {
            spdlog::warn(__FUNCTION__ ": combat target not found, form id {:X}", combatComponent.TargetFormId);
            toRemove.push_back(entity);
            continue;
        }

        const auto& formIdComponent = view.get<FormIdComponent>(entity);
        auto* pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
        if (!pActor)
        {
            spdlog::error(__FUNCTION__ ": actor not found, form id {:X}", formIdComponent.Id);
            toRemove.push_back(entity);
            continue;
        }

        pActor->SetCombatTargetEx(pTarget);
    }

    for (const auto entity : toRemove)
        m_world.remove<CombatComponent>(entity);
}
