#pragma once

#include <Games/ExtraData.h>
#include <Misc/BSFixedString.h>

struct ExtraTextDisplayData : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::TextDisplayData;

    BSFixedString DisplayName{};
    TESForm* pDisplayNameText{};
    TESQuest* pOwnerQuest{};
    int32_t iOwnerInstance{};
    float fTemperFactor{};
    uint16_t usCustomNameLength{};

    // TODO: implement the rest when i dont feel lazy
    /*
    BGSMessage *pDisplayNameText;
    TESQuest *pOwnerQuest;
    uint32_t uiOwnerInstance;
    BSTArray<BSTTuple<BSFixedString,TESForm *>,BSTArrayHeapAllocator> *pTextPairA;
    unsigned __int16 usCustomNameLength;
    */
};

static_assert(sizeof(ExtraTextDisplayData) == 0x38);
