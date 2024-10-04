#pragma once

#include <Structs/ActionEvent.h>

struct ActorExtension
{
    enum
    {
        kRemote = 1 << 0,
        kPlayer = 1 << 1,
    };

    bool IsRemote() const noexcept;
    bool IsLocal() const noexcept;
    bool IsPlayer() const noexcept;
    bool IsRemotePlayer() const noexcept;
    bool IsLocalPlayer() const noexcept;
    void SetRemote(bool aSet) noexcept;
    void SetPlayer(bool aSet) noexcept;

    ActionEvent LatestAnimation{};
    size_t GraphDescriptorHash = 0;
#ifdef MODDED_BEHAVIOR_COMPATIBILITY
    size_t UnmoddedGraphDescriptorHash = 0; // Hash before any mods change it.
    size_t HumanoidGraphDescriptorHash = 0; // Copy of hash before overwritten by beast form.
#endif MODDED_BEHAVIOR_COMPATIBILITY

  private:
    uint32_t onlineFlags{0};
};
