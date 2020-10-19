#pragma once

#include <Actor.h>

struct BGSAction;

struct ActionInput
{
    ActionInput(uint32_t aParam1, Actor* apActor, BGSAction* apAction, TESObjectREFR* apTarget);

    virtual ~ActionInput() { Release(); }

    virtual void sub_1() {}
    virtual void sub_2() {}
    virtual void* sub_3() { return nullptr; }

    void Release();

    GamePtr<Actor> actor; // 8
    GamePtr<TESObjectREFR> target; // 10
    BGSAction* action; // 18
    uint32_t unkInput; // 20
};

