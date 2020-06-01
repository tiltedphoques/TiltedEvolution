#pragma once

template<class T>
struct BSTEventSink
{
    enum Result
    {
        kContinue = 1,
        KAbort
    };

    virtual ~BSTEventSink(){}
    virtual Result HandleEvent(T* apEvent, void* apDispatcher)
    {
        return kContinue;
    }
};
