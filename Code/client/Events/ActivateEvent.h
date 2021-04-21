#pragma once

struct TESObjectREFR;
struct Actor;

struct ActivateEvent
{
    ActivateEvent(TESObjectREFR* apObject, Actor* apActivator, uint8_t aUnk1, int64_t aUnk2, int aUnk3, char aUnk4, bool aActivateFlag = false)
        : pObject(apObject), pActivator(apActivator), unk1(aUnk1), unk2(aUnk2), unk3(aUnk3), unk4(aUnk4), ActivateFlag(aActivateFlag)
    {}

    TESObjectREFR* pObject;
    Actor* pActivator;
    uint8_t unk1;
    int64_t unk2;
    int unk3;
    char unk4;
    bool ActivateFlag;
};
