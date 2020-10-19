#pragma once

#include <Events/EventDispatcher.h>

enum class BSTEventResult
{
    kOk,
    kAbort
};

template<class T>
struct BSTEventSink
{
    virtual ~BSTEventSink() {}
    virtual BSTEventResult OnEvent(const T* apEvent, const EventDispatcher<T>* apSender)
    {
        return BSTEventResult::kOk;
    }
};

