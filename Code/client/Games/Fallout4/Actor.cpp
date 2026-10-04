#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Memory.h>
#include <Forms/ActorValueInfo.h>
#include <Forms/TESNPC.h>
#include <Games/TES.h>

ActorExtension* Actor::GetExtension() noexcept
{
    if (auto* pActor = AsExActor())
        return static_cast<ActorExtension*>(pActor);
    if (auto* pPlayer = AsExPlayerCharacter())
        return static_cast<ActorExtension*>(pPlayer);
    return nullptr;
}

ExActor* Actor::AsExActor() noexcept
{
    return formType == Type && this != PlayerCharacter::Get() ? static_cast<ExActor*>(this) : nullptr;
}

ExPlayerCharacter* Actor::AsExPlayerCharacter() noexcept
{
    return this == PlayerCharacter::Get() ? static_cast<ExPlayerCharacter*>(this) : nullptr;
}

GamePtr<Actor> Actor::New() noexcept
{
    auto* pActor = Memory::Allocate<Actor>();
    if (!pActor)
        return {};
    TP_THIS_FUNCTION(TConstructor, Actor*, Actor, bool);
    static VersionDbPtr<TConstructor> constructor(2229563);
    TiltedPhoques::ThisCall(constructor, pActor, false);
    return {pActor};
}

GamePtr<Actor> Actor::Create(TESNPC* apBaseForm) noexcept
{
    auto* pPlayer = PlayerCharacter::Get();
    auto* pManager = ModManager::Get();
    if (!apBaseForm || !pPlayer || !pManager)
        return {};

    auto pActor = New();
    if (!pActor)
        return {};
    pActor->SetSkipSaveFlag(true);
    pActor->GetExtension()->SetRemote(true);
    pActor->SetObjectReference(apBaseForm);
    pActor->SetParentCell(pPlayer->parentCell);

    auto position = pPlayer->position;
    auto rotation = pPlayer->rotation;
    if (!pManager->Spawn(position, rotation, pPlayer->parentCell, pPlayer->GetWorldSpace(), pActor))
        return {};
    return pActor;
}

GamePtr<Actor> Actor::Spawn(uint32_t aBaseFormId) noexcept
{
    return Create(Cast<TESNPC>(TESForm::GetById(aBaseFormId)));
}

uint16_t Actor::GetLevel() const noexcept
{
    TP_THIS_FUNCTION(TGetLevel, uint16_t, const Actor);
    static VersionDbPtr<TGetLevel> getLevel(2229734);
    return TiltedPhoques::ThisCall(getLevel, this);
}

float Actor::GetActorValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetValue(aId);
}

float Actor::GetActorPermanentValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetPermanentValue(aId);
}

void Actor::SetActorValue(uint32_t aId, float aValue) noexcept
{
    actorValueOwner.SetValue(aId, aValue);
}

void Actor::ForceActorValue(ActorValueOwner::ForceMode aMode, uint32_t aId, float aValue) noexcept
{
    const float initialValue = aMode == ActorValueOwner::ForceMode::PERMANENT ? GetActorPermanentValue(aId) : GetActorValue(aId);
    if (aValue != initialValue)
        actorValueOwner.ForceCurrent(aMode, aId, aValue - initialValue);
}

ActorValues Actor::GetEssentialActorValues() const noexcept
{
    ActorValues values;
    const uint32_t essentialValues[] = {ActorValueInfo::kHealth, ActorValueInfo::kActionPoints, ActorValueInfo::kRads, ActorValueInfo::kRadHealthMax, ActorValueInfo::kFatigue, ActorValueInfo::kFatigueAPMax};
    for (const auto i : essentialValues)
    {
        if (!ActorValueInfo::Resolve(i))
            continue;
        values.ActorValuesList.emplace(i, GetActorValue(i));
        values.ActorMaxValuesList.emplace(i, GetActorPermanentValue(i));
    }
    return values;
}

void Actor::SetActorValues(const ActorValues& acActorValues) noexcept
{
    for (const auto& value : acActorValues.ActorMaxValuesList)
        ForceActorValue(ActorValueOwner::ForceMode::PERMANENT, value.first, value.second);
    for (const auto& value : acActorValues.ActorValuesList)
        ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, value.first, value.second);
}

namespace
{
TP_THIS_FUNCTION(TDestructor, void, Actor);
TDestructor* s_destructor = nullptr;

void TP_MAKE_THISCALL(HookDestructor, Actor)
{
    if (auto* pExtension = apThis->GetExtension())
        pExtension->~ActorExtension();
    TiltedPhoques::ThisCall(s_destructor, apThis);
}

TiltedPhoques::Initializer s_actorHooks(
    []()
    {
        static VersionDbPtr<TDestructor> destructor(2229565);
        s_destructor = destructor.Get();
        TP_HOOK(&s_destructor, HookDestructor);
    });
}
