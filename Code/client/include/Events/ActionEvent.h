#pragma once

struct ActionEvent
{
    uint64_t Tick{0};
    uint32_t ActorId{0};
    uint32_t ActionId{0};
    uint32_t TargetId{0};
    uint32_t IdleId{0};
    uint32_t State1{ 0 };
    uint32_t State2{ 0 };
    float Value{ 0.f };
    uint32_t Type{ 0 };
    Vector<uint32_t> Variables{};

    ActionEvent& operator=(const ActionEvent& acRhs) = default;

    bool operator==(const ActionEvent& acRhs) const noexcept
    {
        return Tick == acRhs.Tick && ActorId == acRhs.ActorId && ActionId == acRhs.ActionId && TargetId == acRhs.TargetId && IdleId == acRhs.IdleId && Variables == acRhs.Variables;
    }

    bool operator!=(const ActionEvent& acRhs) const noexcept
    {
        return !operator==(acRhs);
    }
};
