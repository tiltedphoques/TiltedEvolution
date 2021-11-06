#pragma once

struct AddTargetEvent
{
    AddTargetEvent(uint32_t aTargetID, uint32_t aSpellID) 
        : TargetID(aTargetID), SpellID(aSpellID)
    {}

    uint32_t TargetID;
    uint32_t SpellID;
};
