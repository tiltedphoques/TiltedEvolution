#pragma once

struct BaseFormComponent
{
    virtual ~BaseFormComponent() {};

    virtual void Init() {};
    virtual void ReleaseRefs() {};
    virtual void CopyFromBase(BaseFormComponent* apBase) {};
};
