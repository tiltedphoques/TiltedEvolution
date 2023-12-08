#pragma once

#include <Forms/PlayerCharacter.h>

struct ActorExtension
{
    enum
    {
        kRemote = 1 << 0,
        kPlayer = 1 << 1,
    };

    bool IsRemote() const noexcept { return onlineFlags & kRemote; }

    bool IsLocal() const noexcept { return !IsRemote(); }

    bool IsPlayer() const noexcept { return onlineFlags & kPlayer; }

    bool IsRemotePlayer() const noexcept { return IsRemote() && IsPlayer(); }

    bool IsLocalPlayer() const noexcept { return IsLocal() && IsPlayer(); }

    void SetRemote(bool aSet) noexcept
    {
        if (aSet)
            onlineFlags |= kRemote;
        else
            onlineFlags &= ~kRemote;
    }

    void SetPlayer(bool aSet) noexcept
    {
        if (aSet)
            onlineFlags |= kPlayer;
        else
            onlineFlags &= ~kPlayer;
    }

    // ActionEvent LatestAnimation{};
    // size_t GraphDescriptorHash = 0;

private:
    uint32_t onlineFlags{0};
};

struct ExActor : Actor, ActorExtension
{
};

struct ExPlayerCharacter : PlayerCharacter, ActorExtension
{
};
