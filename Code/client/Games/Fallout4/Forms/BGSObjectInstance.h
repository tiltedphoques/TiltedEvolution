#pragma once

struct TESForm;
struct TBO_InstanceData;

struct BGSObjectInstance
{
    BGSObjectInstance() = delete;
    BGSObjectInstance(TESForm* apObject, TBO_InstanceData* apInstanceData);

    TESForm* pObject;
    TBO_InstanceData* spInstanceData;
};

