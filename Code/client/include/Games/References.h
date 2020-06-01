#pragma once

#include <Games/ActorExtension.h>
#include <Games/Fallout4/PlayerCharacter.h>
#include <Games/Skyrim/PlayerCharacter.h>

struct ExActor : Actor, ActorExtension
{
};

struct ExPlayerCharacter : PlayerCharacter, ActorExtension
{
};

