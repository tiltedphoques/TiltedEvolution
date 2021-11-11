#pragma once

struct AddTargetEvent
{
    AddTargetEvent(uint32_t aTargetID, uint32_t aSpellID) 
        : TargetID(aTargetID), SpellID(aSpellID)
    {
        spdlog::warn("AddTargetEvent, TargetID: {:X}, SpellID: {:X}", aTargetID, aSpellID);
    }

    uint32_t TargetID;
    uint32_t SpellID;
};
