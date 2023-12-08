#pragma once

// This seems to be new. Function ID 72754 locks the pair, 72757 unlocks it.
// The lock function produces an instance of LockPair.
// Used in TESObjectREFR::pInventoryList and TESObjectREFR::pLoadedState.
struct LockPair
{
    uint64_t* pLock;
    void** ppData;
};
