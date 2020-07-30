#pragma once

#ifdef TP_SKYRIM

#include <Games/ExtraData.h>

struct BGSLoadFormBuffer;
struct BGSSaveFormBuffer;
struct TESObjectREFR;
struct TESForm;

struct ExtraContainerChanges : BSExtraData
{
    struct Entry
    {
        TESForm* form;
        GameList<BSExtraDataList>* dataList;
        int32_t count;
        uint8_t pad[8];
    };

    struct Data
    {
        void Save(BGSSaveFormBuffer* apBuffer);
        void Load(BGSLoadFormBuffer* apBuffer);

        GameList<Entry>* entries;
        TESObjectREFR* parent;
        float unk10;
        float unk14; // init to -1.0, maybe weight
        uint16_t unk18; // Maybe count ? init to 0
    };

    Data* data;
};

#endif
