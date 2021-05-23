#pragma once

struct SetStateEvent
{
    SetStateEvent(uint32_t aFormId, const char* aScriptName, const char* aState)
        : formId(aFormId), scriptName(aScriptName), state(aState)
    {
    }

    uint32_t formId;
    String scriptName;
    String state;
};
