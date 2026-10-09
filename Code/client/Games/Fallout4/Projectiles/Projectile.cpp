#include <Projectiles/Projectile.h>

#include <Actor.h>
#include <Games/ActorExtension.h>
#include <Events/ProjectileLaunchedEvent.h>
#include <Forms/TESObjectCELL.h>
#include <World.h>

TP_THIS_FUNCTION(TLaunch, BSPointerHandle<Projectile>*, BSPointerHandle<Projectile>, Projectile::LaunchData& arData);
static TLaunch* RealLaunch = nullptr;

BSPointerHandle<Projectile>* Projectile::Launch(BSPointerHandle<Projectile>* apResult, LaunchData& arData) noexcept
{
    return TiltedPhoques::ThisCall(RealLaunch, apResult, arData);
}

// Projectiles of remote actors only come from the network; local launches are sent there.
BSPointerHandle<Projectile>* TP_MAKE_THISCALL(HookLaunch, BSPointerHandle<Projectile>, Projectile::LaunchData& arData)
{
    if (Actor* pActor = Cast<Actor>(arData.pShooter))
    {
        if (pActor->GetExtension()->IsRemote())
        {
            apThis->handle.iBits = 0;
            return apThis;
        }
    }

    ProjectileLaunchedEvent event{};
    event.Origin = arData.Origin;
    if (arData.pProjectileBase)
        event.ProjectileBaseID = arData.pProjectileBase->formID;
    if (arData.pShooter)
        event.ShooterID = arData.pShooter->formID;
    if (arData.pFromWeapon)
        event.WeaponID = reinterpret_cast<TESForm*>(arData.pFromWeapon)->formID;
    if (arData.pFromAmmo)
        event.AmmoID = reinterpret_cast<TESForm*>(arData.pFromAmmo)->formID;
    event.ZAngle = arData.fZAngle;
    event.XAngle = arData.fXAngle;
    event.YAngle = arData.fYAngle;
    if (arData.pParentCell)
        event.ParentCellID = arData.pParentCell->formID;
    if (arData.pSpell)
        event.SpellID = reinterpret_cast<TESForm*>(arData.pSpell)->formID;
    event.CastingSource = arData.eCastingSource;
    event.Area = arData.iArea;
    event.Power = arData.fPower;
    event.Scale = arData.fScale;
    event.AlwaysHit = arData.bAlwaysHit;
    event.NoDamageOutsideCombat = arData.bNoDamageOutsideCombat;
    event.AutoAim = arData.bAutoAim;
    event.DeferInitialization = arData.bDeferInitialization;
    event.ForceConeOfFire = arData.bForceConeOfFire;
    event.UnkBool1 = arData.bTracer;
    event.UnkBool2 = arData.bPenetrates;

    auto* pResult = TiltedPhoques::ThisCall(RealLaunch, apThis, arData);

    World::Get().GetRunner().Trigger(event);

    return pResult;
}

static TiltedPhoques::Initializer s_projectileHooks(
    []()
    {
        static VersionDbPtr<TLaunch> launch(2236958);
        RealLaunch = launch.Get();
        TP_HOOK(&RealLaunch, HookLaunch);

        // The weapon fire routine reads a flag from the first launched projectile
        // without a null check; blocked launches leave a null entry there.
        static VersionDbPtr<uint8_t> fireWeapon(2198960);
        uint8_t* pLoc = fireWeapon.Get() + 0x8E4;
        constexpr uint8_t kExpected[] = {0x48, 0x8B, 0x03, 0x8B, 0x88, 0x60, 0x02, 0x00, 0x00};
        if (std::memcmp(pLoc, kExpected, sizeof(kExpected)) != 0)
        {
            spdlog::error("Projectile null check not applied, unexpected code at the weapon fire routine");
            return;
        }

        struct C : TiltedPhoques::CodeGenerator
        {
            C(uint8_t* apLoc)
            {
                mov(rax, ptr[rbx]);
                test(rax, rax);
                jz("skip");
                mov(ecx, ptr[rax + 0x260]);
                jmp_S(apLoc + 0x9);
                L("skip");
                jmp_S(apLoc + 0x1A);
            }
        };
        static C gen(pLoc);
        TiltedPhoques::Jump(pLoc, gen.getCode());
    });
