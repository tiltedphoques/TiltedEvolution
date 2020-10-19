#pragma once

#include <Components/BaseFormComponent.h>

struct ActorValueInfo;

struct BGSForcedLocRefType : BaseFormComponent
{
    virtual ~BGSForcedLocRefType();

    ActorValueInfo** values;
};
