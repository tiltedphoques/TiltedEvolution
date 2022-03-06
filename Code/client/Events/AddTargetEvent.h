#pragma once

struct AddTargetEvent
{
    AddTargetEvent() = default;

    uint32_t TargetID{};
    uint32_t SpellID{};
    uint32_t EffectID{};
    float Magnitude{};
};
