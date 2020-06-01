#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct TESForm;

struct BGSDestructibleObjectForm : BaseFormComponent
{
    virtual ~BGSDestructibleObjectForm();

    TESForm* form;
};

#endif
