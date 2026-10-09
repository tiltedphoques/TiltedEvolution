#include <TiltedOnlinePCH.h>

#include <Systems/InterpolationSystem.h>
#include <Components.h>

#include <AI/AIProcess.h>
#include <Misc/MiddleProcess.h>

#include <Games/References.h>
#include <World.h>

namespace
{
// Past the newest snapshot the actor keeps moving for at most this long, then holds still until data arrives
constexpr double cMaxExtrapolation = 150.0;
// Consecutive snapshots further apart than this are a teleport, not movement to interpolate across
constexpr float cTeleportDistance = 1024.f;
// Larger corrections snap instead of sliding the actor into place
constexpr float cMaxPositionCorrection = 256.f;
// Time constant of the correction fading out, ~95% of it is gone after three times this
constexpr double cPositionCorrectionTime = 50.0;
constexpr size_t cMaxTimePoints = 64;
} // namespace

void InterpolationSystem::Update(Actor* apActor, InterpolationComponent& aInterpolationComponent, const double aNow) noexcept
{
    auto& clock = aInterpolationComponent.Clock;
    const double cDeltaTime = clock.Advance(aNow);

    auto& movements = aInterpolationComponent.TimePoints;

    if (movements.size() < 2)
        return;

    const double cRenderTick = clock.GetRenderTick();

    while (movements.size() > 2)
    {
        const auto& second = *(++movements.begin());
        if (cRenderTick >= static_cast<double>(second.Tick))
            movements.pop_front();
        else
            break;
    }

    const auto& first = *(movements.begin());
    const auto& second = *(++movements.begin());

    const auto tickDelta = static_cast<double>(second.Tick - first.Tick);
    const bool cIsTeleport = glm::distance(first.Position, second.Position) > cTeleportDistance;

    glm::vec3 position{};
    auto delta = 1.f;
    bool isExtrapolating = false;

    if (cRenderTick <= static_cast<double>(second.Tick))
    {
        if (tickDelta > 0.0)
            delta = static_cast<float>(std::clamp((cRenderTick - static_cast<double>(first.Tick)) / tickDelta, 0.0, 1.0));

        // Stay at the old position until the teleport happens
        if (cIsTeleport && delta < 1.f)
            delta = 0.f;

        position = TiltedPhoques::Lerp(first.Position, second.Position, delta);
    }
    else
    {
        // The next snapshot is late, keep going the way the actor was moving rather than freezing it
        position = second.Position;

        if (!cIsTeleport && tickDelta > 0.0)
        {
            const double cExtrapolationTime = std::min(cRenderTick - static_cast<double>(second.Tick), cMaxExtrapolation);
            position = TiltedPhoques::Lerp(first.Position, second.Position, static_cast<float>(1.0 + cExtrapolationTime / tickDelta));
            isExtrapolating = true;
        }
    }

    auto& positionError = aInterpolationComponent.PositionError;

    // The late snapshot arrived, so the actor is no longer where extrapolation put it
    if (aInterpolationComponent.HasNewTimePoint && aInterpolationComponent.IsExtrapolating)
        positionError = aInterpolationComponent.Position - position;

    if (cIsTeleport || glm::length(positionError) > cMaxPositionCorrection)
        positionError = {};
    else
        positionError *= static_cast<float>(std::exp(-cDeltaTime / cPositionCorrectionTime));

    aInterpolationComponent.HasNewTimePoint = false;
    aInterpolationComponent.IsExtrapolating = isExtrapolating;
    aInterpolationComponent.Position = position + positionError;

    // Don't try to move a null actor
    if (!apActor)
        return;

    apActor->ForcePosition(aInterpolationComponent.Position);
    apActor->LoadAnimationVariables(second.Variables);

    if (apActor->currentProcess && apActor->currentProcess->middleProcess)
    {
        apActor->currentProcess->middleProcess->direction = second.Direction;
    }

    auto rotA = first.Rotation;
    auto rotB = second.Rotation;

    const auto deltaX = TiltedPhoques::DeltaAngle(rotA.x, rotB.x, true) * delta;
    const auto deltaY = TiltedPhoques::DeltaAngle(rotA.y, rotB.y, true) * delta;
    const auto deltaZ = TiltedPhoques::DeltaAngle(rotA.z, rotB.z, true) * delta;

    auto finalX = TiltedPhoques::Mod(rotA.x + deltaX, float(TiltedPhoques::Pi * 2));
    if (finalX > 0.f && finalX > float(TiltedPhoques::Pi / 2))
        finalX -= TiltedPhoques::Pi * 2;

    const auto finalY = TiltedPhoques::Mod(rotA.y + deltaY, float(TiltedPhoques::Pi * 2));
    const auto finalZ = TiltedPhoques::Mod(rotA.z + deltaZ, float(TiltedPhoques::Pi * 2));

    apActor->SetRotation(finalX, finalY, finalZ);
}

void InterpolationSystem::AddPoint(InterpolationComponent& aInterpolationComponent, InterpolationComponent::TimePoint aPoint, const double aArrivalTime) noexcept
{
    if (!aInterpolationComponent.Clock.AddSample(aArrivalTime, aPoint.Tick))
        return;

    auto& timePoints = aInterpolationComponent.TimePoints;

    // Snapshots are unreliable and can arrive out of order
    auto itor = std::begin(timePoints);
    while (itor != std::end(timePoints) && itor->Tick < aPoint.Tick)
        ++itor;

    timePoints.insert(itor, std::move(aPoint));
    aInterpolationComponent.HasNewTimePoint = true;

    while (timePoints.size() > cMaxTimePoints)
        timePoints.pop_front();
}

uint64_t InterpolationSystem::GetPlaybackTick(const InterpolationComponent& acInterpolationComponent) noexcept
{
    // The render tick stays at 0 until the first snapshot arrives
    return static_cast<uint64_t>(std::max(acInterpolationComponent.Clock.GetRenderTick(), 0.0));
}

InterpolationComponent& InterpolationSystem::Setup(World& aWorld, const entt::entity aEntity) noexcept
{
    return aWorld.emplace_or_replace<InterpolationComponent>(aEntity);
}

void InterpolationSystem::Clean(World& aWorld, const entt::entity aEntity) noexcept
{
    if (aWorld.all_of<InterpolationComponent>(aEntity))
        aWorld.remove<InterpolationComponent>(aEntity);
}
