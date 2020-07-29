#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Events/EventDispatcher.h>

template<class T>
struct BSTEventSink
{
    enum Result
    {
        kAbort = 0,
        kOk = 1
    };

    virtual ~BSTEventSink() = 0;
    virtual Result OnEvent(const T* apEvent, const EventDispatcher<T>* apSender) = 0;
};

#endif
