
#include <Structs/TimeModel.h>

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
