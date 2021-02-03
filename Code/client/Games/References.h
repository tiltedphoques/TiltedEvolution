#pragma once

#include <Games/ActorExtension.h>
#include <PlayerCharacter.h>

struct ExActor : Actor, ActorExtension
{
};

struct ExPlayerCharacter : PlayerCharacter, ActorExtension
{
};

