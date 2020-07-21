#pragma once

#include <Buffer.hpp>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/Rotator2_NetQuantize.h>

using TiltedPhoques::Buffer;

struct Movement
{
    Movement() = default;
    ~Movement() = default;

    bool operator==(const Movement& acRhs) const noexcept;
    bool operator!=(const Movement& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    Vector3_NetQuantize Position{};
    Rotator2_NetQuantize Rotation{};
};
