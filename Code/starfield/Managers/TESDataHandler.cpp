#include "TESDataHandler.h"

#include <Forms/Actor.h>

TESDataHandler* TESDataHandler::Get()
{
    return (TESDataHandler*)0x145924378; // 825890
}

BSPointerHandle<TESObjectREFR>
TESDataHandler::CreateReferenceAtLocation(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apCharacter)
{
    using TCreateReferenceAtLocation = BSPointerHandle<TESObjectREFR>(__fastcall)(TESDataHandler*, BSPointerHandle<TESObjectREFR>*, NEW_REFR_DATA*);
    TCreateReferenceAtLocation* createReferenceAtLocation = (TCreateReferenceAtLocation*)0x1414984DC; // 85126

    NEW_REFR_DATA data{};
    data.location = aPosition;
    data.direction = aRotation;
    data.pBaseForm = apCharacter->pBaseForm;
    data.pRefrToPlaceMaybe = apCharacter;
    data.pParentCell = apParentCell;
    data.pWorld = apWorldSpace;

    BSPointerHandle<TESObjectREFR> result{};
    return createReferenceAtLocation(this, &result, &data);
}
