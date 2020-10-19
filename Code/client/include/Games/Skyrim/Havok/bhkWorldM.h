#pragma once

struct ahkpWorld;

struct bhkWorldM
{
    virtual ~bhkWorldM();

    virtual void sub_01();
    virtual void sub_02();
    virtual void sub_03();
    virtual void sub_04();
    virtual void sub_05();
    virtual void sub_06();
    virtual void sub_07();
    virtual void sub_08();
    virtual void sub_09();
    virtual void sub_0A();
    virtual void sub_0B();
    virtual void sub_0C();
    virtual void sub_0D();
    virtual void sub_0E();
    virtual void sub_0F();
    virtual void sub_10();
    virtual void sub_11();
    virtual void sub_12();
    virtual void sub_13();
    virtual void sub_14();
    virtual void sub_15();
    virtual void sub_16();
    virtual void sub_17();
    virtual void sub_18();
    virtual void sub_19();
    virtual void sub_1A();
    virtual void sub_1B();
    virtual void sub_1C();
    virtual void sub_1D();
    virtual void sub_1E();
    virtual void sub_1F();
    virtual void sub_20();
    virtual void sub_21();
    virtual void sub_22();
    virtual void sub_23();
    virtual void sub_24();
    virtual void sub_25();
    virtual void sub_26();
    virtual void sub_27();
    virtual ahkpWorld* GetWorld() { return hkpWorld; }

    uint8_t pad8[0x10 - 0x8];
    ahkpWorld* hkpWorld; // 10
};
