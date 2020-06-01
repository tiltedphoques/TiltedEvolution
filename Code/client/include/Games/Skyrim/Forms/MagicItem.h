#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESBoundObject.h>

#include <Games/Skyrim/Components/BGSKeywordForm.h>
#include <Games/Skyrim/Components/TESFullName.h>

struct EffectSetting;

struct MagicItem : TESBoundObject
{
    TESFullName fullName;
    BGSKeywordForm keyword;
    GameArray<void*> unk34;
    uint32_t unk40;
    EffectSetting* unk44;
    uint32_t unk48;
    uint32_t unk4C;
};

#endif
