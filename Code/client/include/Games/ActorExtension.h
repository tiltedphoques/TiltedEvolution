#pragma once

struct ActorExtension
{
    enum
    {
        kRemote = 1 << 0
    };

    void InitializeExtension() noexcept;

    bool IsRemote() const noexcept;
    void SetRemote(bool aSet) noexcept;

    uint32_t onlineFlags;
};
