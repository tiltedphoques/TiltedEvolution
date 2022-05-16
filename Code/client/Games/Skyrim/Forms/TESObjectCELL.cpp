#include <Forms/TESObjectCELL.h>
#include <Games/ExtraData.h>
#include <TESObjectREFR.h>

static BSExtraData* (*TESObjectCELL_GetRegionList)(TESObjectCELL*, bool){nullptr};

Vector<TESObjectREFR*> TESObjectCELL::GetRefsByFormTypes(const Vector<FormType>& aFormTypes) noexcept
{
    Vector<TESObjectREFR*> references{};

    if (!refData.refArray)
        return references;

    for (uint32_t i = 0; i < refData.capacity; i++)
    {
        TESObjectREFR* pRef = refData.refArray[i].Get();
        if (!pRef)
            continue;

        for (FormType formType : aFormTypes)
        {
            if (pRef->baseForm->formType == formType)
                references.push_back(pRef);
        }
    }

    return references;
}

void TESObjectCELL::GetCOCPlacementInfo(NiPoint3* aOutPos, NiPoint3* aOutRot, bool aAllowCellLoad) noexcept
{
    TP_THIS_FUNCTION(TGetCOCPlacementInfo, void, TESObjectCELL, NiPoint3*, NiPoint3*, bool);
    POINTER_SKYRIMSE(TGetCOCPlacementInfo, s_getCOCPlacementInfo, 19075);
    ThisCall(s_getCOCPlacementInfo, this, aOutPos, aOutRot, aAllowCellLoad);
}

BSExtraData* TESObjectCELL::GetRegionList(bool abCreateIfNotFound)
{
    return TESObjectCELL_GetRegionList(this, abCreateIfNotFound);
}

static TiltedPhoques::Initializer s_InitCell([]() {
    const VersionDbPtr<uint8_t> s_getregions(18999);
    TESObjectCELL_GetRegionList = static_cast<decltype(TESObjectCELL_GetRegionList)>(s_getregions.GetPtr());
});