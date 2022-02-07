#pragma once

struct Actor;
struct BGSPerk;

struct TaskQueueInterface
{
    static TaskQueueInterface* Get() noexcept;
    void QueueApplyPerk(Actor* apActor, BGSPerk* apPerk, int8_t aOldRank, int8_t aNewRank) noexcept;
};
