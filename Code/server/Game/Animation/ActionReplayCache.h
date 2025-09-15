#pragma once

#include <Structs/ActionEvent.h>
#include <Structs/ActionReplayChain.h>
#include <optional>

//
// Note: the _best_ solution to the 'proper animation state' problem is to implement animation graphs serialization,
// and then send the graphs over the network. But since that is a pretty hard reverse engineering task,
// let's do simple action replays until better days.
//

struct ActionReplayCache
{
    static constexpr uint32_t kReplayCacheMaxSize = 32;

    ActionReplayChain GetReplayChain() const noexcept;

    const Vector<ActionEvent>& GetActions() const noexcept { return m_actions; };

    /// Whether clients should reset the animation graph of the Actor before replaying
    bool IsGraphResetNeeded() const noexcept { return m_isGraphResetNeeded; };

    /// Appends actions and refines (optimizes) the replay cache immediately after
    void AppendAll(const Vector<ActionEvent>& acActions) noexcept;

private:
    void RefineReplayCache() noexcept;
    static bool IsExitAction(const ActionEvent& acAction) noexcept;
    static bool ShouldIgnoreAction(const ActionEvent& acAction) noexcept;
    static std::optional<String> FindInstantCounterpartForAction(const String& acAction) noexcept;

    Vector<ActionEvent> m_actions{};
    bool m_isGraphResetNeeded{false};
};
