#pragma once

struct CombatComponent
{
    CombatComponent() = delete;
    CombatComponent(uint32_t aTargetFormId)
        : TargetFormId(aTargetFormId)
    {
    }

    uint32_t TargetFormId;
    float Timer = 3.0f;
};
