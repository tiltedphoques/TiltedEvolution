#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct ActorValueInfo;

struct BGSForcedLocRefType : BaseFormComponent
{
    virtual ~BGSForcedLocRefType();

    ActorValueInfo** values;
};


#endif
