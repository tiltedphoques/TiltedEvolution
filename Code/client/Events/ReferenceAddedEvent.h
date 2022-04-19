#pragma once

#include <Forms/TESForm.h>

struct ReferenceAddedEvent
{
    explicit ReferenceAddedEvent(const uint32_t aFormId, const FormType aFormType)
        : FormId(aFormId)
        , FormType(aFormType)
    {}

    uint32_t FormId;
    FormType FormType;
};
