#pragma once

struct BaseFormComponent
{
    virtual ~BaseFormComponent();

    virtual void Init();
    virtual void ReleaseRefs();
    virtual void CopyFromBase(BaseFormComponent* apBase);
    virtual void sub_4();
    virtual void sub_5();
    virtual void sub_6();
};
