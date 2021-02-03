#pragma once

#include <Components/BaseFormComponent.h>

struct TESForm;

struct BGSDestructibleObjectForm : BaseFormComponent
{
    virtual ~BGSDestructibleObjectForm();

    TESForm* form;
};
