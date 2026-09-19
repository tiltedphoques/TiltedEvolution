#pragma once

struct TESBoundObject;

struct GarbageCollector
{
    static GarbageCollector* Get() noexcept;

    // Uses the engine's immediate/deferred base-object deletion policy.
    void Add(TESBoundObject* apObject) noexcept;
};
