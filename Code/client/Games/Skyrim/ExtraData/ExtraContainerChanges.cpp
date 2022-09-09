#include <TiltedOnlinePCH.h>

#include <ExtraData/ExtraContainerChanges.h>

void ExtraContainerChanges::Data::Save(BGSSaveFormBuffer* apBuffer)
{
    TP_THIS_FUNCTION(TSaveFunc, void*, ExtraContainerChanges::Data, BGSSaveFormBuffer*);

    POINTER_SKYRIMSE(TSaveFunc, s_save, 16142);

    TiltedPhoques::ThisCall(s_save, this, apBuffer);
}

void ExtraContainerChanges::Data::Load(BGSLoadFormBuffer* apBuffer)
{
    TP_THIS_FUNCTION(TLoadFunc, void*, ExtraContainerChanges::Data, BGSLoadFormBuffer*);

    POINTER_SKYRIMSE(TLoadFunc, s_load, 16143);

    TiltedPhoques::ThisCall(s_load, this, apBuffer);
}

bool ExtraContainerChanges::Entry::IsQuestObject() noexcept
{
    TP_THIS_FUNCTION(TIsQuestObject, bool, ExtraContainerChanges::Entry);

    POINTER_SKYRIMSE(TIsQuestObject, s_isQuestObject, 16005);

    return TiltedPhoques::ThisCall(s_isQuestObject, this);
}
