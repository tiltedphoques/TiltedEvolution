#pragma once

struct TESActionData;

struct ActorMediator
{
    virtual ~ActorMediator(){};

    static ActorMediator* Get() noexcept;

    bool PerformAction(TESActionData* apAction) noexcept;
    bool ForceAction(TESActionData* apAction) noexcept;
};
