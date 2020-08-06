#pragma once

#include <cstdint>

struct TimeModel
{
    float TimeScale = 20.f;
    float Time = 12.f;
    int Year = 1;
    int Month = 1;
    int Day = 1;

    inline void Update(uint64_t delta)
    {
        float deltaSeconds = static_cast<float>(delta) / 1000.f;
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

    inline int GetNumerOfDaysByMonthIndex(int index)
    {
        if (index < 12)
        {
            return cDayLengthArray[index];
        }

        return 0;
    }

private:
    const int cDayLengthArray[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
};
