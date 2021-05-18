#pragma once

#include <Games/ActorExtension.h>
#include <PlayerCharacter.h>

struct ExActor : Actor, ActorExtension
{
};

struct ExPlayerCharacter : PlayerCharacter, ActorExtension
{
};

struct CRC32
{
    static uint32_t GenerateCRC(uint64_t aData) noexcept;
};
