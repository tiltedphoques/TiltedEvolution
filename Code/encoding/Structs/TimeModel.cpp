
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

void TimeModel::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    // poor man's std::bitcast
    aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&TimeScale), 32);
    aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&Time), 32);
    aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&Day), 32);
    aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&Month), 32);
    aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&Year), 32);
}

void TimeModel::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t tmp = 0;
    uint32_t cVal = 0;

    aReader.ReadBits(tmp, 32);
    cVal = tmp & 0xFFFFFFFF;
    TimeScale = *reinterpret_cast<float*>(&cVal);

    aReader.ReadBits(tmp, 32);
    cVal = tmp & 0xFFFFFFFF;
    Time = *reinterpret_cast<float*>(&cVal);

    aReader.ReadBits(tmp, 32);
    cVal = tmp & 0xFFFFFFFF;
    Day = *reinterpret_cast<int*>(&cVal);

    aReader.ReadBits(tmp, 32);
    cVal = tmp & 0xFFFFFFFF;
    Month = *reinterpret_cast<int*>(&cVal);

    aReader.ReadBits(tmp, 32);
    cVal = tmp & 0xFFFFFFFF;
    Year = *reinterpret_cast<int*>(&cVal);
}

bool TimeModel::operator==(const TimeModel& achRhs) const noexcept
{
    return TimeScale == achRhs.TimeScale &&
        Time == achRhs.Time &&
        Day == achRhs.Day &&
        Month == achRhs.Month &&
        Year == achRhs.Year;
}

bool TimeModel::operator!=(const TimeModel& achRhs) const noexcept { return !this->operator==(achRhs); }
