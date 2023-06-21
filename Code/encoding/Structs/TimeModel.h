#pragma once

#include <cstdint>

using TiltedPhoques::Buffer;

struct TimeModel
{
    // Default time: 01/01/01 at 12:00
    float TimeScale = 20.f;
    float Time = 12.f;
    uint32_t Year = 1;
    uint32_t Month = 1;
    uint32_t Day = 1;

    void Update(uint64_t aDelta);
    float GetTimeInDays() const noexcept;

    [[nodiscard]] static uint32_t GetNumberOfDaysByMonthIndex(int index);

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    std::string ToString() const noexcept;

    bool operator==(const TimeModel& achRhs) const noexcept { return TimeScale == achRhs.TimeScale && Time == achRhs.Time && Day == achRhs.Day && Month == achRhs.Month && Year == achRhs.Year; }
    bool operator!=(const TimeModel& achRhs) const noexcept { return !this->operator==(achRhs); }
};
