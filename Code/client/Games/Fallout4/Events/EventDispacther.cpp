#include <TiltedOnlinePCH.h>

#include <Events/EventDispatcher.h>

namespace details
{
    void InternalRegisterSink(void* apEventDispatcher, void* apSink) noexcept
    {
        TP_THIS_FUNCTION(TRegisterSink, void, void, void* apSink);

        // GameVM ctor RegisterSinks
        POINTER_FALLOUT4(TRegisterSink, s_registerSink, 57457);

        ThisCall(s_registerSink, apEventDispatcher, apSink);
    }

    void InternalUnRegisterSink(void* apEventDispatcher, void* apSink) noexcept
    {
        TP_THIS_FUNCTION(TUnRegisterSink, void, void, void* apSink);

        // GameVM dtor UnRegisterSinks
        POINTER_FALLOUT4(TUnRegisterSink, s_unregisterSink, 1089538);

        ThisCall(s_unregisterSink, apEventDispatcher, apSink);
    }

    void InternalPushEvent(void* apEventDispatcher, void* apEvent) noexcept
    {
        TP_THIS_FUNCTION(TPushEvent, void, void, void* apSink);

        // "Failed to setup moving reference because it has no parent cell or no 3D" last function's content before the call
        POINTER_FALLOUT4(TPushEvent, s_pushEvent, 646456);

        ThisCall(s_pushEvent, apEventDispatcher, apEvent);
    }
}

