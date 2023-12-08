#include "TESObjectREFR.h"

#include <Managers/GameVM.h>

TESObjectREFR* TESObjectREFR::GetByHandle(uint32_t aHandle) noexcept
{
    TESObjectREFR* pResult = nullptr;

    using TGetRefrByHandle = void(TESObjectREFR** apResult, uint32_t* apHandle);
    TGetRefrByHandle* getRefrByHandle = (TGetRefrByHandle*)0x141254a54; // 72399

    getRefrByHandle(&pResult, &aHandle);

    if (pResult)
    {
        uint64_t newRefCount = pResult->DecRefCount();
        if (newRefCount == 0)
            return nullptr;
    }
    
    return pResult;
}

BSPointerHandle<TESObjectREFR> TESObjectREFR::GetHandle() noexcept
{
    using TGetHandle = BSPointerHandle<TESObjectREFR>(__fastcall)(TESObjectREFR*, BSPointerHandle<TESObjectREFR>*);
    TGetHandle* getHandle = (TGetHandle*)0x141A05078;

    BSPointerHandle<TESObjectREFR> result{};
    getHandle(this, &result);

    return result;
}

void TESObjectREFR::Delete() noexcept
{
    using TDelete = void(IVirtualMachine*, uint64_t, TESObjectREFR*);
    TDelete* deleteFunc = (TDelete*)0x142A7E6B0; // 172352
    deleteFunc(GameVM::Get()->pVM, 0, this);
}

void TESObjectREFR::Reset() noexcept
{
    using TReset = void(IVirtualMachine*, uint64_t, TESObjectREFR*, TESObjectREFR*);
    TReset* reset = (TReset*)0x142A87310; // 172371
    reset(GameVM::Get()->pVM, 0, this, nullptr);
}

TESWorldSpace* TESObjectREFR::GetWorldSpace() noexcept
{
    using TGetWorldSpace = TESWorldSpace*(TESObjectREFR*);
    TGetWorldSpace* getWorldSpace = (TGetWorldSpace*)0x141a093fc; // 106696
    return getWorldSpace(this);
}

REFR_LOCK* TESObjectREFR::GetLock() noexcept
{
    using TGetLock = REFR_LOCK*(TESObjectREFR*);
    TGetLock* getLock = (TGetLock*)0x141A37ADC; // 107581
    return getLock(this);
}

REFR_LOCK* TESObjectREFR::AddLock() noexcept
{
    using TAddLock = REFR_LOCK*(TESObjectREFR*);
    TAddLock* addLock = (TAddLock*)0x141A34FCC; // 107503
    return addLock(this);
}

// TODO: hook this
void TESObjectREFR::AddLockChange() noexcept
{
    using TAddLockChange = void(TESObjectREFR*);
    TAddLockChange* addLockChange = (TAddLockChange*)0x1419FA2A4; // 106386
    addLockChange(this);
}

// TODO: hook this
bool TESObjectREFR::ActivateRef(REFR_ACTIVATE_DATA* apData) noexcept
{
    using TActivateREFR = bool(TESObjectREFR*, REFR_ACTIVATE_DATA*);
    TActivateREFR* activateREFR = (TActivateREFR*)0x1419F86EC; // 106374
    return activateREFR(this, apData);
}

// TODO: hook this
// This is the same as HookSetPosition in tilted evo
bool HookSetLocationOnReference(TESObjectREFR* apThis, NiPoint3& aPosition)
{
    using TSetLocationOnReference = bool(TESObjectREFR*, NiPoint3&);
    TSetLocationOnReference* setLocationOnReference = (TSetLocationOnReference*)0x141A1D234; // 106932
    return setLocationOnReference(apThis, aPosition);
}

void TESObjectREFR::RemoveAllItems(TESObjectREFR* apTargetContainer, bool aChangeOwnership, bool aIgnoreWeightLimit) noexcept
{
    using TRemoveAllItems = void(TESObjectREFR* apThis, TESObjectREFR* apTargetContainer, bool aChangeOwnership, bool aIgnoreWeightLimit);
    TRemoveAllItems* removeAllItems = (TRemoveAllItems*)0x141A1803C; // 106852
    removeAllItems(this, apTargetContainer, aChangeOwnership, aIgnoreWeightLimit);
}
