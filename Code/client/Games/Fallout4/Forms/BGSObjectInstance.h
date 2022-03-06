#pragma once

struct TESForm;
struct TBO_InstanceData;

struct BGSObjectInstance
{
    BGSObjectInstance(TESForm* apObject, TBO_InstanceData* apInstanceData);

    TESForm* pObject;
    void* spInstanceData; // BSTSmartPointer<TBO_InstanceData,BSTSmartPointerIntrusiveRefCount> spInstanceData;
};

