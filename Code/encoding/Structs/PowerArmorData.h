#pragma once

#include <Structs/Inventory.h>

struct PowerArmorData
{
    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    bool operator==(const PowerArmorData& acOther) const noexcept;
    bool IsValid() const noexcept;

    Inventory Items{};
    float BatteryCharge{};
    uint32_t FrameToken{};
};
