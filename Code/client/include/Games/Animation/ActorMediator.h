#pragma once

struct TESActionData;

struct ActorMediator
{
    virtual ~ActorMediator() {};

    static ActorMediator* Get() noexcept;

    bool PerformAction(TESActionData* apAction, float aValue) noexcept;

    // Reverse engineering stuff, do not use
    uint8_t RePerformAction(TESActionData* apAction, float aValue) noexcept;
    void RePerformIdleAction(TESActionData* apData, float aValue) noexcept;
    uintptr_t RePerformComplexAction(TESActionData* apData, float aValue) noexcept;
};
