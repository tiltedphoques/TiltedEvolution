#pragma once

struct TESObjectREFR;
struct Actor;
struct TESBoundObject;

/**
 * @brief In-game activation (i.e. interacting with an object).
 */
struct ActivateEvent
{
    ActivateEvent(TESObjectREFR* apObject, Actor* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, uint8_t aUnk1, TESObjectREFR::OpenState aPreActivationOpenState, bool aActivateFlag = false)
        : pObject(apObject)
        , pActivator(apActivator)
        , pObjectToGet(apObjectToGet)
        , Count(aCount)
        , DefaultProcessing(aDefaultProcessing)
        , Unk1(aUnk1)
        , PreActivationOpenState(aPreActivationOpenState)
        , ActivateFlag(aActivateFlag)
    {
    }

    TESObjectREFR* pObject;
    Actor* pActivator;
    TESBoundObject* pObjectToGet;
    int32_t Count;
    bool DefaultProcessing;
    uint8_t Unk1;
    TESObjectREFR::OpenState PreActivationOpenState;
    bool ActivateFlag;
};
