#pragma once

#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::Map;

struct ActorValues
{
    ActorValues() = default;
    ~ActorValues() = default;

    bool operator==(const ActorValues& acRhs) const noexcept;
    bool operator!=(const ActorValues& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    Map<uint32_t, float> ActorValuesList{};
    Map<uint32_t, float> ActorMaxValuesList{};
};
