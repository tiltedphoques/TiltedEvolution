#pragma once

#include <Games/ActorExtension.h>
#include <PlayerCharacter.h>

namespace GameplayFormulas
{
float CalculateRealDamage(Actor* apHittee, float aDamage) noexcept;
}

namespace Settings
{
int32_t* GetDifficulty() noexcept;
float* GetGreetDistance() noexcept;
}

void FadeOutGame(bool aFadingOut, bool aBlackFade, float aFadeDuration, bool aRemainVisible, float aSecondsToFade) noexcept;

struct ExActor : Actor, ActorExtension
{
};

struct ExPlayerCharacter : PlayerCharacter, ActorExtension
{
};

