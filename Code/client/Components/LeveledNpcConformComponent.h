#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct Actor;

// Marks the disable/enable cycle used to apply a leveled actor's canonical base.
// Removal listeners ignore the temporary disappearance. Component checks and
// IsReenabling guards suspend animation, appearance, and actor movement updates
// until the cycle finishes or is cleaned up.
struct LeveledNpcConformComponent
{
    Actor* ExpectedActor{};
    uint32_t ActorFormId{};
};
