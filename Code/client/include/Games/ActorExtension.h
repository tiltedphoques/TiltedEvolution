#pragma once

#include <ActionEvent.h>

struct ActorExtension
{
    enum
    {
        kRemote = 1 << 0
    };

    bool IsRemote() const noexcept;
    void SetRemote(bool aSet) noexcept;

    ActionEvent LatestAnimation{};
    ActionEvent LatestVariables{};

private:

    uint32_t onlineFlags{ 0 };
};
