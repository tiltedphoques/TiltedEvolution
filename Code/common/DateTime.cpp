#include "DateTime.h"
#include <cmath>

bool DateTime::operator==(const DateTime& other) const
{
    return m_timeModel == other.m_timeModel;
}

const uint32_t cDayLengthArray[12] = {31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u};

uint32_t DateTime::GetNumberOfDaysByMonthIndex(int aIndex)
{
    return cDayLengthArray[aIndex % 12];
}

void DateTime::Update(uint64_t aDeltaTick)
{
    m_timeModel.Time += GetDeltaTime(aDeltaTick);

    m_timeModel.Day += static_cast<uint32_t>(m_timeModel.Time / 24.f);
    m_timeModel.Time = std::fmod(m_timeModel.Time, 24.f);

    while (m_timeModel.Day > GetNumberOfDaysByMonthIndex(m_timeModel.Month))
    {
        m_timeModel.Day -= GetNumberOfDaysByMonthIndex(m_timeModel.Month);
        m_timeModel.Month++;
    }
    m_timeModel.Year += m_timeModel.Month / 12;
    m_timeModel.Month %= 12;
}

float DateTime::GetDeltaTime(uint64_t aDeltaTick) const noexcept
{
    float deltaSeconds = static_cast<float>(aDeltaTick) / 1000.f;
    return (deltaSeconds * (m_timeModel.TimeScale * 0.00027777778f));
}

float DateTime::GetTimeInDays() const noexcept
{
    float totalDays = static_cast<float>(m_timeModel.Year) * 365.f;
    for (uint32_t i = 0u; i < m_timeModel.Month; i++)
        totalDays += GetNumberOfDaysByMonthIndex(i);
    totalDays += m_timeModel.Day;
    totalDays += (m_timeModel.Time / 24.f);
    return totalDays;
}
