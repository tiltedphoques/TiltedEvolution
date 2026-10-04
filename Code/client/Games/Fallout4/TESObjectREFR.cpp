#include <TiltedOnlinePCH.h>

#include <TESObjectREFR.h>
#include <Forms/TESObjectCELL.h>

TESObjectREFR* TESObjectREFR::GetByHandle(uint32_t aHandle) noexcept
{
    using TGetReference = bool(const uint32_t&, TESObjectREFR*&);
    static VersionDbPtr<TGetReference> getReference(2188681);
    TESObjectREFR* pReference = nullptr;
    getReference.Get()(aHandle, pReference);
    if (pReference)
        pReference->handleRefObject.DecRefHandle();
    return pReference;
}

BSPointerHandle<TESObjectREFR> TESObjectREFR::GetHandle() const noexcept
{
    TP_THIS_FUNCTION(TGetHandle, BSPointerHandle<TESObjectREFR>*, const TESObjectREFR, BSPointerHandle<TESObjectREFR>*);
    static VersionDbPtr<TGetHandle> getHandle(2201196);
    BSPointerHandle<TESObjectREFR> handle;
    TiltedPhoques::ThisCall(getHandle, this, &handle);
    return handle;
}

uint32_t* TESObjectREFR::GetNullHandle() noexcept
{
    static VersionDbPtr<uint32_t> nullHandle(4795988);
    return nullHandle.Get();
}

TESObjectCELL* TESObjectREFR::GetParentCellEx() const noexcept
{
    return parentCell ? parentCell : GetSaveParentCell();
}

uint32_t TESObjectREFR::GetCellId() const noexcept
{
    const auto* pCell = GetParentCellEx();
    return pCell ? pCell->formID : 0;
}

TESWorldSpace* TESObjectREFR::GetWorldSpace() const noexcept
{
    const auto* pCell = GetParentCellEx();
    return pCell && !(pCell->cellFlags & 1) ? pCell->worldspace : nullptr;
}

ExtraDataList* TESObjectREFR::GetExtraDataList() noexcept
{
    return extraData.object;
}

Lock* TESObjectREFR::GetLock() const noexcept
{
    TP_THIS_FUNCTION(TGetLock, Lock*, const TESObjectREFR);
    static VersionDbPtr<TGetLock> getLock(2202648);
    return TiltedPhoques::ThisCall(getLock, this);
}

TESContainer* TESObjectREFR::GetContainer() const noexcept
{
    TP_THIS_FUNCTION(TGetContainer, TESContainer*, const TESObjectREFR);
    static VersionDbPtr<TGetContainer> getContainer(2201022);
    return TiltedPhoques::ThisCall(getContainer, this);
}

int64_t TESObjectREFR::GetItemCountInInventory(TESForm* apItem) const noexcept
{
    TP_THIS_FUNCTION(TGetCount, bool, const TESObjectREFR, uint32_t&, TESForm*, bool);
    static VersionDbPtr<TGetCount> getCount(2200996);
    uint32_t count = 0;
    TiltedPhoques::ThisCall(getCount, this, count, apItem, false);
    return count;
}

void TESObjectREFR::Enable() const noexcept
{
    TP_THIS_FUNCTION(TEnable, void, const TESObjectREFR, bool);
    static VersionDbPtr<TEnable> enable(2201150);
    TiltedPhoques::ThisCall(enable, this, false);
}

void TESObjectREFR::SetRotation(float aX, float aY, float aZ) noexcept
{
    TP_THIS_FUNCTION(TSetAngle, void, TESObjectREFR, const NiPoint3&);
    static VersionDbPtr<TSetAngle> setAngle(2201134);
    const NiPoint3 angle(glm::vec3(aX, aY, aZ));
    TiltedPhoques::ThisCall(setAngle, this, angle);
}

const float TESObjectREFR::GetHeight() noexcept
{
    return GetBoundMax().z - GetBoundMin().z;
}
