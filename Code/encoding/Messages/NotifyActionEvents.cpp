#include <Messages/NotifyActionEvents.h>
#include <TiltedCore/Serialization.hpp>

void NotifyActionEvents::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Actions.size());

    for (const auto& [serverId, actions] : Actions)
    {
        Serialization::WriteVarInt(aWriter, serverId);
        Serialization::WriteVarInt(aWriter, actions.size());

        for (const auto& action : actions)
            action.GenerateDifferential(ActionEvent{}, aWriter);
    }
}

void NotifyActionEvents::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    const auto count = Serialization::ReadVarInt(aReader);

    for (auto i = 0u; i < count; ++i)
    {
        const uint32_t cServerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
        auto& actions = Actions[cServerId];

        actions.resize(Serialization::ReadVarInt(aReader));

        for (auto& action : actions)
            action.ApplyDifferential(aReader);
    }
}
