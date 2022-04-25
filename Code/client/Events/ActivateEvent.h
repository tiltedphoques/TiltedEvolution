#pragma once

struct TESObjectREFR;
struct Actor;
struct TESBoundObject;

/**
* @brief In-game activation (i.e. interacting with an object).
*/
struct ActivateEvent
{
#if TP_FALLOUT4
    ActivateEvent(TESObjectREFR* apObject, Actor* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping, bool aActivateFlag = false)
        : pObject(apObject), pActivator(apActivator), pObjectToGet(apObjectToGet), Count(aCount), DefaultProcessing(aDefaultProcessing), FromScript(aFromScript), IsLooping(aIsLooping), ActivateFlag(aActivateFlag)
    {}
#elif TP_SKYRIM64
    ActivateEvent(TESObjectREFR* apObject, Actor* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, uint8_t aUnk1, bool aActivateFlag = false)
        : pObject(apObject), pActivator(apActivator), pObjectToGet(apObjectToGet), Count(aCount), DefaultProcessing(aDefaultProcessing), Unk1(aUnk1), ActivateFlag(aActivateFlag)
    {}
#endif

    TESObjectREFR* pObject;
    Actor* pActivator;
    TESBoundObject* pObjectToGet;
    int32_t Count;
    bool DefaultProcessing;
#if TP_FALLOUT4
    bool FromScript;
    bool IsLooping;
#elif TP_SKYRIM64
    uint8_t Unk1;
#endif

    bool ActivateFlag;
};
