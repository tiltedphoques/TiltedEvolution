#include <Systems/InterpolationSystem.h>
#include <Components.h>

#include <Games/Skyrim/Misc/ActorProcessManager.h>
#include <Games/Skyrim/Misc/MiddleProcess.h>

#include <Games/Fallout4/Misc/ProcessManager.h>
#include <Games/Fallout4/Misc/MiddleProcess.h>

#include <Games/References.h>
#include <World.h>

void InterpolationSystem::Update(Actor* apActor, InterpolationComponent& aInterpolationComponent, const uint64_t aTick) noexcept
{
    auto& movements = aInterpolationComponent.TimePoints;

    while (movements.size() > 2)
    {
        const auto second = *(++movements.begin());
        if (aTick > second.Tick)
            movements.pop_front();
        else
            break;
    }

    if (movements.size() < 2)
    {
        auto pName = apActor->GetName();
        pName = pName != nullptr ? pName : "UNKNOWN";

        //spdlog::warn("Actor {s} is missing interpolation points, skipping frame.", pName);
        return;
    }

    const auto& first = *(movements.begin());
    const auto& second = *(++movements.begin());

    // Calculate delta movement since last update
    auto delta = 0.0001f;
    const auto tickDelta = float(second.Tick - first.Tick);
    if (tickDelta > 0.f)
    {
        delta = 1.f / tickDelta * float(aTick - first.Tick);
    }

    delta = TiltedPhoques::Min(delta, 1.0f);

    NiPoint3 position{};
    TiltedPhoques::Lerp(first.Position, second.Position, delta)
        .Decompose(position.x, position.y, position.z);
    
    apActor->ForcePosition(position);

    float firstX, firstY, firstZ;
    float secondX, secondY, secondZ;

    first.Rotation.Decompose(firstX, firstY, firstZ);
    second.Rotation.Decompose(secondX, secondY, secondZ);

    const auto deltaX = TiltedPhoques::DeltaAngle(firstX, secondX, true) * delta;
    const auto deltaY = TiltedPhoques::DeltaAngle(firstY, secondY, true) * delta;
    const auto deltaZ = TiltedPhoques::DeltaAngle(firstZ, secondZ, true) * delta;

    const auto finalX = TiltedPhoques::Mod(firstX + deltaX, TiltedPhoques::Pi * 2);
    const auto finalY = TiltedPhoques::Mod(firstY + deltaY, TiltedPhoques::Pi * 2);
    const auto finalZ = TiltedPhoques::Mod(firstZ + deltaZ, TiltedPhoques::Pi * 2);

    apActor->SetRotation(finalX, finalY, finalZ);
}

void InterpolationSystem::AddPoint(InterpolationComponent& aInterpolationComponent,
    const InterpolationComponent::TimePoint& acPoint) noexcept
{
    auto itor = std::begin(aInterpolationComponent.TimePoints);
    const auto end = std::cend(aInterpolationComponent.TimePoints);

    while(itor != end)
    {
        if(itor->Tick > acPoint.Tick)
        {
            aInterpolationComponent.TimePoints.insert(itor, acPoint);

            return;
        }

        ++itor;
    }

    aInterpolationComponent.TimePoints.push_back(acPoint);
}

void InterpolationSystem::Setup(World& aWorld, const entt::entity aEntity) noexcept
{
    aWorld.emplace<InterpolationComponent>(aEntity);
}
