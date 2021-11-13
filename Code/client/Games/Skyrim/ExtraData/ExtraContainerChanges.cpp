#include <TiltedOnlinePCH.h>

#include <ExtraData/ExtraContainerChanges.h>

void ExtraContainerChanges::Data::Save(BGSSaveFormBuffer* apBuffer)
{
    TP_THIS_FUNCTION(TSaveFunc, void*, ExtraContainerChanges::Data, BGSSaveFormBuffer*);

    POINTER_SKYRIMSE(TSaveFunc, s_save, 0x1401F7CB0 - 0x140000000);

    ThisCall(s_save, this, apBuffer);
}

void ExtraContainerChanges::Data::Load(BGSLoadFormBuffer* apBuffer)
{
    TP_THIS_FUNCTION(TLoadFunc, void*, ExtraContainerChanges::Data, BGSLoadFormBuffer*);

    POINTER_SKYRIMSE(TLoadFunc, s_load, 0x1401F7DE0 - 0x140000000);

    ThisCall(s_load, this, apBuffer);
}
