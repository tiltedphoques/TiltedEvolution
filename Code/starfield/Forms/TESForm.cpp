#include "TESForm.h"

TESForm* TESForm::GetById(uint32_t aId)
{
    using TGetById = TESForm*(uint32_t);
    TGetById* getById = (TGetById*)0x1414D7E20; // 86125
    return getById(aId);
}

void TESForm::SetSkipSaveFlag(bool aSet)
{
    if (aSet)
    {
        sInGameFormFlags = 0xFFFF; // TODO: verify
    }
}
