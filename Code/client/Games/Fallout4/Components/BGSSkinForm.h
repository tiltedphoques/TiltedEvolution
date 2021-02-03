#pragma once

#include <Components/BaseFormComponent.h>

struct TESForm;

struct BGSSkinForm : BaseFormComponent
{
    virtual ~BGSSkinForm();

    TESForm* form;
};
