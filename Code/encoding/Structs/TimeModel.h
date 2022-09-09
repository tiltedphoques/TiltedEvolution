#pragma once

#include <cstdint>

using TiltedPhoques::Buffer;

struct TimeModel
{
    // Default time: 01/01/01 at 12:00
    float TimeScale = 20.f;
    float Time = 12.f;
    int Year = 1;
    int Month = 1;
    int Day = 1;

    bool operator==(const TimeModel& acRhs) const noexcept;
    bool operator!=(const TimeModel& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    void Update(uint64_t aDelta);
    [[nodiscard]] static int GetNumerOfDaysByMonthIndex(int index);
};
