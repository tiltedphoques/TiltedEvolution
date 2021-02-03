
#include <Structs/QuestLog.h>

#include <TiltedCore/Serialization.hpp>
#include <Structs/Mods.h>
#include <algorithm>

using TiltedPhoques::Serialization;

bool QuestLog::operator==(const QuestLog& acRhs) const noexcept
{
    return Entries == acRhs.Entries;
}

bool QuestLog::operator!=(const QuestLog& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void QuestLog::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Entries.size() & 0xFFFF, 16);
    for (auto& e : Entries)
    {
        e.Id.Serialize(aWriter);

        // id's can be in the full 16 bit range
        aWriter.WriteBits(e.Stage, 16);
    }
}

void QuestLog::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t temp;
    aReader.ReadBits(temp, 16);

    const size_t count = temp & 0xFFFF;
    Entries.resize(count);

    for (auto& e : Entries)
    {
        e.Id.Deserialize(aReader);
        aReader.ReadBits(temp, 16);

        e.Stage = temp & 0xFFFF;
    }
}
