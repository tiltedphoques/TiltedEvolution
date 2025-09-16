#pragma once

#include <Structs/ActionEvent.h>
#include <Structs/ActionReplayChain.h>
#include <optional>

//
// Note: the _best_ solution to the 'proper animation state' problem is to implement animation graphs serialization,
// and then send the graphs over the network. But since that is a pretty hard reverse engineering task,
// let's do simple action replays until better days.
//

class ActionReplayCache
{
public:
    ActionReplayCache() = default;
    ~ActionReplayCache() = default;

    static constexpr uint32_t kReplayCacheMaxSize = 32;

    ActionReplayChain FormRefinedReplayChain() noexcept;

    /// Appends actions to the replay cache
    void AppendAll(const Vector<ActionEvent>& acActions) noexcept;

    const Vector<ActionEvent>& GetActions() const noexcept { return m_actions; };

private:
    // Returns true if clients should reset the animation graph of the Actor before replaying
    bool RefineReplayCache() noexcept;
    static bool IsExitAction(const ActionEvent& acAction) noexcept;
    static bool ShouldIgnoreAction(const ActionEvent& acAction) noexcept;
    static std::optional<String> FindInstantCounterpartForAction(const String& acAction) noexcept;

    Vector<ActionEvent> m_actions{};
};
