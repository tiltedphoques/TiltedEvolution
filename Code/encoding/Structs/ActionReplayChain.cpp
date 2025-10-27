#include <Structs/ActionReplayChain.h>

using TiltedPhoques::Serialization;

bool ActionReplayChain::operator==(const ActionReplayChain& acRhs) const noexcept
{
    return ResetAnimationGraph == acRhs.ResetAnimationGraph && Actions == acRhs.Actions;
}

bool ActionReplayChain::operator!=(const ActionReplayChain& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void ActionReplayChain::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, ResetAnimationGraph);
    aWriter.WriteBits(Actions.size() & 0xFF, 8);
    for (size_t i = 0; i < Actions.size(); ++i)
    {
        Actions[i].GenerateDifferential(ActionEvent{}, aWriter);
    }
}

void ActionReplayChain::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ResetAnimationGraph = Serialization::ReadBool(aReader);
    uint64_t actionsCount = 0;
    aReader.ReadBits(actionsCount, 8);
    Actions.resize(actionsCount);
    for (ActionEvent& replayAction : Actions)
    {
        replayAction.ApplyDifferential(aReader);
    }
}
