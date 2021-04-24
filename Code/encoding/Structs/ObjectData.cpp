#include <Structs/ObjectData.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool ObjectData::operator==(const ObjectData& acRhs) const noexcept
{
    return Id == acRhs.Id &&
           CellId == acRhs.CellId &&
           CurrentLockData == acRhs.CurrentLockData;
}

bool ObjectData::operator!=(const ObjectData& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void ObjectData::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    CellId.Serialize(aWriter);
    CurrentLockData.Serialize(aWriter);
}

void ObjectData::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Id.Deserialize(aReader);
    CellId.Deserialize(aReader);
    CurrentLockData.Deserialize(aReader);
}
