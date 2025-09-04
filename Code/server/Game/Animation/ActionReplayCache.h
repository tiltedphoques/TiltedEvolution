#pragma once

#include <Structs/ActionEvent.h>

//
// Note: the _best_ solution to the 'proper animation state' problem is to implement animation graphs serialization, 
// and then send the graphs over the network. But since that is a pretty hard reverse engineering task, 
// let's do simple action replays until better days.
//

struct ActionReplayCache
{
    static constexpr uint32_t kReplayCacheMaxSize = 32;

    // TODO: Consider std::array<Action, 32> since the size is constant
    Vector<ActionEvent> Actions;

    /**
     * @brief Appends actions and truncates the replay chain immediately after. 
     * Either: 1) action chain (aka replay cache) will start with an Exit action,
     * 2) chain will start with a valid Start action,
     * 3) chain will remain unchanged.
     */
    void AppendAll(const Vector<ActionEvent>& acActions) noexcept;

private:
    void TruncateReplayChain() noexcept;
    static bool IsStartAction(const ActionEvent& acAction) noexcept;
    static bool IsExitAction(const ActionEvent& acAction) noexcept;
    static bool ShouldSkipIntermediateAction(const ActionEvent& acAction) noexcept;
};
