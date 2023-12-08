#pragma once

struct BaseFormComponent
{
    virtual ~BaseFormComponent();
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
};

static_assert(sizeof(BaseFormComponent) == 0x8);
