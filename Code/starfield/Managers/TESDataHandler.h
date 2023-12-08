#pragma once

#include <Forms/TESObjectREFR.h>
#include <BSCore/NiPoint3.h>

struct TESWorldSpace;
struct Actor;

struct TESDataHandler
{
    struct NEW_REFR_DATA
    {
        // TODO: Not sure which one to use yet
        /*
        NEW_REFR_DATA(
            TESForm* apBaseForm, void* a2, void* a3, void* a4, void* a5, TESWorldSpace* apWorld, void* a7, void* a8, void* a9, bool a10, bool a11, uint32_t* a12, void* a13, bool a14, bool a15,
            bool a16, bool a17);
        */
        NEW_REFR_DATA()
        {
            using TNEW_REFR_DATA_ctor = NEW_REFR_DATA*(NEW_REFR_DATA*);
            TNEW_REFR_DATA_ctor* ctor = (TNEW_REFR_DATA_ctor*)0x141371C5C; // 130061
            ctor(this);
        }

        virtual void unk0();

        uint8_t unk8[8];
        NiPoint3 location;
        NiPoint3 direction;
        float scaleMaybe;
        uint32_t pad2C;
        TESForm* pBaseForm;
        TESObjectCELL* pParentCell;
        TESWorldSpace* pWorld;
        TESForm* pRefrToPlaceMaybe;
        uint8_t unk50[0x70 - 0x50];
        void* pSomeExtraDataThing;
        uint64_t unk78;
        void* pModExtra;
        // ...
    };

    static TESDataHandler* Get();
    
    BSPointerHandle<TESObjectREFR>
    CreateReferenceAtLocation(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apCharacter);

    uint8_t unk0[0x1740];
};

static_assert(offsetof(TESDataHandler::NEW_REFR_DATA, pBaseForm) == 0x30);
static_assert(offsetof(TESDataHandler::NEW_REFR_DATA, pModExtra) == 0x80);
static_assert(sizeof(TESDataHandler) == 0x1740);
