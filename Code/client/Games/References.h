#pragma once

#include <Games/ActorExtension.h>
#include <PlayerCharacter.h>

namespace GameplayFormulas
{

float CalculateRealDamage(Actor* apHittee, float aDamage) noexcept;

}

namespace Settings
{

int32_t* GetDifficulty() noexcept
{
    POINTER_SKYRIMSE(int32_t, s_difficulty, 381472);
    return s_difficulty.Get();
}

float* GetGreetDistance() noexcept
{
    POINTER_SKYRIMSE(float, s_greetDistance, 370892);
    return s_greetDistance.Get();
}

}

struct ExActor : Actor, ActorExtension
{
};

struct ExPlayerCharacter : PlayerCharacter, ActorExtension
{
};

