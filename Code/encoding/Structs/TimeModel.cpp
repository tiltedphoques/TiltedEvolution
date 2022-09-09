
#include <Structs/TimeModel.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

const int cDayLengthArray[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int TimeModel::GetNumerOfDaysByMonthIndex(int aIndex)
{
    if (aIndex < 12)
    {
        return cDayLengthArray[aIndex];
    }

    return 0;
}

bool TimeModel::operator==(const TimeModel& acRhs) const noexcept
{
    return TimeScale == acRhs.TimeScale &&
        Time == acRhs.Time &&
        Year == acRhs.Year &&
        Month == acRhs.Month &&
        Day == acRhs.Day;
}

bool TimeModel::operator!=(const TimeModel& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void TimeModel::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, TimeScale);
    Serialization::WriteFloat(aWriter, Time);
    Serialization::WriteVarInt(aWriter, Year);
    Serialization::WriteVarInt(aWriter, Month);
    Serialization::WriteVarInt(aWriter, Day);
}

void TimeModel::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    TimeScale = Serialization::ReadFloat(aReader);
    Time = Serialization::ReadFloat(aReader);
    Year = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Month = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Day = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}

void TimeModel::Update(uint64_t aDelta)
{
    float deltaSeconds = static_cast<float>(aDelta) / 1000.f;
    Time += (deltaSeconds * (TimeScale * 0.00027777778f));

    if (Time > 24.f)
    {
        int maxDays = GetNumerOfDaysByMonthIndex(Month);

        while (Time > 24.f)
        {
            Time = Time + -24.f;
            Day++;
        }

        if (Day > maxDays)
        {
            Month++;
            Day = Day - maxDays;

            if (Month > 12)
            {
                Month = Month + -12;
                Year++;
            }
        }
    }
}
