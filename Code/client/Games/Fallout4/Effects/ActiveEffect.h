#pragma once

struct ActiveEffect
{
    virtual void sub_0();
    virtual void sub_1();
    virtual void sub_2();
    virtual void sub_3();
    virtual void sub_4();
    virtual void sub_5();
    virtual void sub_6();
    virtual void sub_7();
    virtual void sub_8();
    virtual void sub_9();
    virtual void sub_A();
    virtual void sub_B();
    virtual void sub_C();
    virtual void sub_D();
    virtual void sub_E();
    virtual void sub_F();
    virtual void sub_10();
    virtual void sub_11();
    virtual void sub_12();
    virtual void sub_13();
    virtual void sub_14();
    virtual void sub_15();
    virtual void sub_16();
    virtual void sub_17();
    virtual void sub_18();

    char pad_0008[112];
    float effectValue;
    char pad_007C[4];
};
static_assert(sizeof(ActiveEffect) == 0x80);
