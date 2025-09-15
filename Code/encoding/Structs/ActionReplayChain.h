#pragma once

#include <Structs/ActionEvent.h>

using TiltedPhoques::Vector;

struct ActionReplayChain
{
    ActionReplayChain() = default;
    ~ActionReplayChain() = default;

    bool operator==(const ActionReplayChain& acRhs) const noexcept;
    bool operator!=(const ActionReplayChain& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    bool ResetAnimationGraph{false};
    Vector<ActionEvent> Actions;
};
