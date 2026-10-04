#include <TiltedOnlinePCH.h>

#include <Forms/TESObjectCELL.h>
#include <Forms/TESBoundObject.h>
#include <TESObjectREFR.h>

Vector<TESObjectREFR*> TESObjectCELL::GetRefsByFormTypes(const Vector<FormType>& aFormTypes) const noexcept
{
    Vector<TESObjectREFR*> references;
    for (auto* pReference : objectList)
    {
        if (pReference && pReference->baseForm && std::find(aFormTypes.begin(), aFormTypes.end(), pReference->baseForm->formType) != aFormTypes.end())
            references.push_back(pReference);
    }
    return references;
}

void TESObjectCELL::GetCOCPlacementInfo(NiPoint3* aOutPos, NiPoint3* aOutRot, bool aAllowCellLoad) noexcept
{
    TP_THIS_FUNCTION(TGetPlacement, void, TESObjectCELL, NiPoint3*, NiPoint3*, bool);
    static VersionDbPtr<TGetPlacement> getPlacement(2200343);
    TiltedPhoques::ThisCall(getPlacement, this, aOutPos, aOutRot, aAllowCellLoad);
}
