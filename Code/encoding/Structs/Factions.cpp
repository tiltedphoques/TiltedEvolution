#include <Structs/Factions.h>
#include <TiltedCore/Serialization.hpp>
#include <stdexcept>

using TiltedPhoques::Serialization;

bool Factions::operator==(const Factions& acRhs) const noexcept
{
    return NpcFactions == acRhs.NpcFactions &&
        ExtraFactions == acRhs.ExtraFactions;
}

bool Factions::operator!=(const Factions& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Factions::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    // Limit the number of factions to send
    Serialization::WriteVarInt(aWriter, NpcFactions.size() & 0x1FF);

    for (auto& entry : NpcFactions)
    {
        entry.Serialize(aWriter);
    }

    Serialization::WriteVarInt(aWriter, ExtraFactions.size() & 0x1FF);

    for (auto& entry : ExtraFactions)
    {
        entry.Serialize(aWriter);
    }
}

void Factions::Deserialize(TiltedPhoques::Buffer::Reader& aReader)
{
    auto npcCount = Serialization::ReadVarInt(aReader);
    if (npcCount > 0x1FF)
        throw std::runtime_error("Too many NPC factions received !");

    NpcFactions.resize(npcCount);
    for (auto& entry : NpcFactions)
    {
        entry.Deserialize(aReader);
    }

    auto extraCount = Serialization::ReadVarInt(aReader) & 0x1FF;
    if (extraCount > 0x1FF)
        throw std::runtime_error("Too many extra factions received !");

    ExtraFactions.resize(extraCount);
    for (auto& entry : ExtraFactions)
    {
        entry.Deserialize(aReader);
    }
}
