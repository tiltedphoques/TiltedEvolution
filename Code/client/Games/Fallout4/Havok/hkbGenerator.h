#pragma once

struct hkEventContext;

struct hkbGenerator
{
    virtual ~hkbGenerator();

    virtual void sub_01();
    virtual void sub_02();
    virtual void sub_03();
    virtual void sub_04();
    virtual void sub_05();
    virtual void SendEvent(hkEventContext& aContext, hkEventType& aType);
};
