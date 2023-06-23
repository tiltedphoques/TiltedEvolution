#include <Structs/TimeModel.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

const uint32_t cDayLengthArray[12] = {31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u};

uint32_t TimeModel::GetNumberOfDaysByMonthIndex(int aIndex)
{
    return cDayLengthArray[aIndex % 12];
}

void TimeModel::Update(uint64_t aDeltaTick)
{
    Time += GetDeltaTime(aDeltaTick);

    Day += static_cast<uint32_t>(Time / 24.f);
    Time = std::fmod(Time, 24.f);

    while (Day > GetNumberOfDaysByMonthIndex(Month))
    {
        Day -= GetNumberOfDaysByMonthIndex(Month);
        Month++;
    }
    Year += Month / 12;
    Month %= 12;
}

float TimeModel::GetDeltaTime(uint64_t aDeltaTick) const noexcept
{
    float deltaSeconds = static_cast<float>(aDeltaTick) / 1000.f;
    return (deltaSeconds * (TimeScale * 0.00027777778f));
}

float TimeModel::GetTimeInDays() const noexcept
{
    float totalDays = static_cast<float>(Year) * 365.f;
    for (uint32_t i = 0u; i < Month; i++)
        totalDays += GetNumberOfDaysByMonthIndex(i);
    totalDays += Day;
    totalDays += (Time / 24.f);
    return totalDays;
}

void TimeModel::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, TimeScale);
    Serialization::WriteFloat(aWriter, Time);
    Serialization::WriteVarInt(aWriter, Day);
    Serialization::WriteVarInt(aWriter, Month);
    Serialization::WriteVarInt(aWriter, Year);
}

void TimeModel::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    TimeScale = Serialization::ReadFloat(aReader);
    Time = Serialization::ReadFloat(aReader);
    Day = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Month = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Year = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
