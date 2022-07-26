#include <Structs/ReferenceUpdate.h>
#include <TiltedCore/Serialization.hpp>
#include <stdexcept>

using TiltedPhoques::Serialization;

bool ReferenceUpdate::operator==(const ReferenceUpdate& acRhs) const noexcept
{
    return UpdatedMovement == acRhs.UpdatedMovement &&
        ActionEvents == acRhs.ActionEvents;
}

bool ReferenceUpdate::operator!=(const ReferenceUpdate& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void ReferenceUpdate::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    UpdatedMovement.Serialize(aWriter);
    
    Serialization::WriteVarInt(aWriter, ActionEvents.size());

    for (auto& entry : ActionEvents)
    {
        entry.GenerateDifferential(ActionEvent{}, aWriter);
    }
}

void ReferenceUpdate::Deserialize(TiltedPhoques::Buffer::Reader& aReader)
{
    UpdatedMovement.Deserialize(aReader);

    const auto count = Serialization::ReadVarInt(aReader);
    // TODO: keeps throwing in fallout together with more than 2 players
    if (count > 0x100)
        throw std::runtime_error("Too many reference updates received !");

    ActionEvents.resize(count);

    for (auto i = 0u; i < count; ++i)
    {
        ActionEvents[i].ApplyDifferential(aReader);
    }
}
