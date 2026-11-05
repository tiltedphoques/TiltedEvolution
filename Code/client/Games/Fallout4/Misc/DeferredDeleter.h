#pragma once

struct TESBoundObject;

struct DeferredDeleter
{
    // Base-object disposal used by Actor::RecalcLeveledActor.
    static void Add(TESBoundObject* apObject) noexcept;
};
