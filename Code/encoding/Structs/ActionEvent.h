#pragma once

#include <cstdint>
#include "AnimationVariables.h"
#include "CachedString.h"

using TiltedPhoques::String;

struct ActionEvent
{
    uint64_t Tick{ 0 };
    uint32_t ActorId{ 0 };
    uint32_t ActionId{ 0 };
    uint32_t TargetId{ 0 };
    uint32_t IdleId{ 0 };
    uint32_t State1{ 0 };
    uint32_t State2{ 0 };
    uint32_t Type{ 0 };
    CachedString EventName{};
    CachedString TargetEventName{};
    AnimationVariables Variables{};

    ActionEvent() = default;
    ActionEvent(const ActionEvent& acRhs) = default;
    ActionEvent(ActionEvent&& acRhs) = default;
    ~ActionEvent() = default;
    ActionEvent& operator=(const ActionEvent& acRhs) = default;
    ActionEvent& operator=(ActionEvent&& acRhs) = default;

    bool operator==(const ActionEvent& acRhs) const noexcept;
    bool operator!=(const ActionEvent& acRhs) const noexcept;

    void Load(std::istream&);
    void Save(std::ostream&) const;

    void GenerateDifferential(const ActionEvent& aPrevious, TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void ApplyDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept;
};
