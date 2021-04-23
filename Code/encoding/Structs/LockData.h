#pragma once

#include <TiltedCore/Buffer.hpp>

struct LockData
{
    LockData() = default;
    ~LockData() = default;

    bool operator==(const LockData& acRhs) const noexcept;
    bool operator!=(const LockData& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    uint8_t IsLocked{};
    uint8_t LockLevel{};
};
