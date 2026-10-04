#pragma once

#include <Games/Primitives.h>

struct NiRTTI;
struct NiTriBasedGeom;

struct NiObject : NiRefObject
{
    virtual ~NiObject();

    virtual NiRTTI* GetRTTI();

    virtual void sub_3();
    virtual void sub_4();
    virtual void sub_5();
    virtual void sub_6();
    virtual void sub_7();
    virtual void sub_8();
    virtual NiTriBasedGeom* CastToNiTriBasedGeom();
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
};
