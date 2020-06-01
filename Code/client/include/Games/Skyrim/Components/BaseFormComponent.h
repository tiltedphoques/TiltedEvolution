#pragma once

#if defined(TP_SKYRIM)

struct BaseFormComponent
{
    virtual ~BaseFormComponent();

    virtual void Init();
    virtual void ReleaseRefs();
    virtual void CopyFromBase(BaseFormComponent* apBase);
};

#endif
