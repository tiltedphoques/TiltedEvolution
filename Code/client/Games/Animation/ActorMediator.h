#pragma once

struct TESActionData;

struct ActorMediator
{
    virtual ~ActorMediator() {};

    static ActorMediator* Get() noexcept;

    bool PerformAction(TESActionData* apAction) noexcept;
    bool ForceAction(TESActionData* apAction) noexcept;

    // Reverse engineering stuff, do not use
    bool RePerformAction(TESActionData* apAction) noexcept;
    bool RePerformIdleAction(TESActionData* apData) noexcept;
    bool RePerformComplexAction(TESActionData* apData) noexcept;
};
