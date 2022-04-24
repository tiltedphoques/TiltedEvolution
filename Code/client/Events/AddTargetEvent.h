#pragma once

/**
* @brief Magic effect added to a target (i.e. an actor or object).
*/
struct AddTargetEvent
{
    AddTargetEvent() = default;

    uint32_t TargetID{};
    uint32_t SpellID{};
    uint32_t EffectID{};
    float Magnitude{};
};
