#pragma once

struct RemoveSpellEvent
{
    RemoveSpellEvent() = default;

    uint32_t TargetId{};
    uint32_t SpellId{};
};
