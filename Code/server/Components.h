#pragma once

// This makes sure we can't include components any other way than by including this file
#define TP_INTERNAL_COMPONENTS_GUARD

#include <Components/ModsComponent.h>
#include <Components/FormIdComponent.h>
#include <Components/OwnerComponent.h>
#include <Components/CellIdComponent.h>
#include <Components/CharacterComponent.h>
#include <Components/MovementComponent.h>
#include <Components/AnimationComponent.h>
#include <Components/InventoryComponent.h>
#include <Components/QuestLogComponent.h>
#include <Components/PartyComponent.h>
#include <Components/ActorValuesComponent.h>
#include <Components/ObjectComponent.h>

#undef TP_INTERNAL_COMPONENTS_GUARD
