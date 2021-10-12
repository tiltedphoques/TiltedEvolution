#pragma once

#include <Structs/ActionEvent.h>

struct ActorExtension
{
    enum
    {
        kRemote = 1 << 0
    };

    bool IsRemote() const noexcept;
    bool IsLocal() const noexcept;
    void SetRemote(bool aSet) noexcept;

    ActionEvent LatestAnimation{};
    size_t GraphDescriptorHash = 0;

private:

    uint32_t onlineFlags{ 0 };
};
