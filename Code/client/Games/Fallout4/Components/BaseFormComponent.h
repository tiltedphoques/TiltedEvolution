#pragma once

struct TESForm;

struct BaseFormComponent
{
    virtual ~BaseFormComponent() = default;
    virtual uint32_t GetFormComponentType() const;
    virtual void Init();
    virtual void ReleaseRefs();
    virtual void InitComponent();
    virtual void CopyComponent(BaseFormComponent* apBase, TESForm* apOwner);
    virtual void CopyFromBase(BaseFormComponent* apBase);
};
