#pragma once

struct GetVariablesEvent
{
    GetVariablesEvent(uint32_t aFormId)
        : formId(aFormId)
    {
    }

    uint32_t formId;
};
