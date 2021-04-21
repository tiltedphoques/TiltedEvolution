#pragma once

// This makes sure we can't include components any other way than by including this file
#define TP_INTERNAL_COMPONENTS_GUARD

#include <Components/LocalAnimationComponent.h>
#include <Components/RemoteAnimationComponent.h>
#include <Components/FormIdComponent.h>
#include <Components/InterpolationComponent.h>
#include <Components/WaitingForAssignmentComponent.h>
#include <Components/LocalComponent.h>
#include <Components/RemoteComponent.h>
#include <Components/FaceGenComponent.h>
#include <Components/CacheComponent.h>
#include <Components/WaitingFor3D.h>
#include <Components/ActorValuesComponent.h>
#include <Components/InteractiveObjectComponent.h>

#undef TP_INTERNAL_COMPONENTS_GUARD
