#pragma once

struct TESConditionItem
{
    enum Function : uint16_t
    {
        kGetStage = 58,
        kGetStageDone = 59,
    };

    enum class OpCode : uint8_t
    {
        kEqualTo,
        kNotEqualTo,
        kGreaterThan,
        kGreaterThanOrEqualTo,
        kLessThan,
        kLessThanOrEqualTo,
    };

    [[nodiscard]] OpCode GetOpCode() const noexcept { return static_cast<OpCode>(flags >> 5); }
    // comparisonValue holds a TESGlobal* instead of a float
    [[nodiscard]] bool UsesGlobal() const noexcept { return flags & 4; }

    TESConditionItem* pNext;
    float comparisonValue; // 0x08
    uint32_t pad0C;
    uint32_t runOnRef;
    uint32_t dataId;
    uint16_t function; // 0x18
    uint16_t pad1A;
    uint32_t pad1C;
    void* params[2]; // 0x20
    uint8_t flags;   // 0x30, bit 0 = OR with the next item, bit 2 = global, bits 5-7 = OpCode
    uint8_t object;
};

static_assert(offsetof(TESConditionItem, comparisonValue) == 0x8);
static_assert(offsetof(TESConditionItem, function) == 0x18);
static_assert(offsetof(TESConditionItem, params) == 0x20);
static_assert(offsetof(TESConditionItem, flags) == 0x30);

struct TESCondition
{
    TESConditionItem* pHead{};
};
