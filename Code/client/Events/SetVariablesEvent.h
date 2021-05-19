#pragma once

struct SetVariablesEvent
{
    SetVariablesEvent(uint32_t aFormId, const char* aScriptName, const char* aVariableName, int aNewValue)
        : formId(aFormId), scriptName(aScriptName), variableName(aVariableName), newValue(aNewValue)
    {
    }

    uint32_t formId;
    String scriptName;
    String variableName;
    int newValue;
};
