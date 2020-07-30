#pragma once

#include <Games/Skyrim/Events/EventDispatcher.h>
#include <Games/Fallout4/Events/EventDispatcher.h>

template<class T>
struct BSTEventSink
{
    enum Result
    {
        kOk = 0,
        kAbort = 1
    };

    virtual ~BSTEventSink() {}
    virtual Result OnEvent(const T* apEvent, const EventDispatcher<T>* apSender) { return kOk; }
};

