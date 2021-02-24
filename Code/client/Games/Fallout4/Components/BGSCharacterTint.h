#pragma once

struct BGSCharacterTint
{
    struct Entry
    {
        virtual ~Entry();
        virtual void sub_1();
        virtual void sub_2();
        virtual void sub_3();
        virtual void sub_4();
        virtual void sub_5();
        virtual void sub_6();

        void* unk8;
        uint16_t unk10; // 10 - type maybe ? copied from ctor
        uint8_t unk12; // 12 - set to 0 in ctor
    };
};
