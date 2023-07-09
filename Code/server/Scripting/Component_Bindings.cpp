
#include "GameServer.h"

namespace Script
{
namespace
{
void BindMovementComponent(sol::state_view aState)
{
    aState["GetMovementComponent"] = [](entt::entity aEntity) {
        return GameServer::Get()->GetWorld().try_get<MovementComponent>(aEntity);
    };
        
    auto table =
        aState.new_usertype<MovementComponent>("MovementComponent", sol::constructors<MovementComponent()>());
    table["Tick"] = &MovementComponent::Tick;
    table["Position"] = &MovementComponent::Position;
    table["Rotation"] = &MovementComponent::Rotation;
    // movementComponentType["Variables"] = &MovementComponent::Variables;
    table["Direction"] = &MovementComponent::Direction;
    table["Sent"] = &MovementComponent::Sent;
}
} // namespace

void CreateComponentBindings(sol::state_view aState)
{
    BindMovementComponent(aState);
}
} // namespace Script
