#pragma once

#include "ExtraDataList.h"

struct BGSLoadFormBuffer;
struct BGSSaveFormBuffer;
struct TESObjectREFR;
struct TESForm;

struct ExtraContainerChanges : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::ContainerChanges;

    struct Entry
    {
        bool IsQuestObject() noexcept;

        TESForm* form;
        GameList<ExtraDataList>* dataList;
        int32_t count;
        uint8_t pad[8];
    };

    struct Data
    {
        void Save(BGSSaveFormBuffer* apBuffer);
        void Load(BGSLoadFormBuffer* apBuffer);

        GameList<Entry>* entries;
        TESObjectREFR* parent;
        float totalWeight;
        float armorWeight; // init to -1.0, maybe weight
        uint16_t unk18; // Maybe count ? init to 0
    };

    Data* data;
};
