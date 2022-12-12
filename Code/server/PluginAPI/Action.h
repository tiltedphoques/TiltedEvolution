// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <PluginAPI/APIBoundary.h>
#include <PluginAPI/Slice.h>

namespace PluginAPI
{
enum class ArgType : uint8_t
{
    kUnknown,
    kBool,
    kU32,
    KI32,
    kU64,
    kI64,
    kF32,
    kString,
};

constexpr const char* ArgTypeToString(ArgType type)
{
    switch (type)
    {
    case ArgType::kUnknown: return "unknown";
    case ArgType::kBool: return "bool";
    case ArgType::kU32: return "u32";
    case ArgType::KI32: return "i32";
    case ArgType::kU64: return "u64";
    case ArgType::kI64: return "i64";
    case ArgType::kF32: return "f32";
    case ArgType::kString: return "string";
    default: return "";
    }
}

template <class _Ty, class... _Types>
static constexpr bool IsAnyOf = // true if and only if _Ty is in _Types
    std::disjunction_v<std::is_same<_Ty, _Types>...>;

template <typename T> static constexpr ArgType ToArgType()
{
    if constexpr (std::is_same_v<T, bool>)
        return ArgType::kBool;
    if constexpr (std::is_same_v<T, uint32_t>)
        return ArgType::kU32;
    if constexpr (std::is_same_v<T, int32_t>)
        return ArgType::KI32;
    if constexpr (std::is_same_v<T, uint64_t>)
        return ArgType::kU64;
    if constexpr (std::is_same_v<T, int64_t>)
        return ArgType::kI64;
    if constexpr (std::is_same_v<T, float>)
        return ArgType::kF32;
    return ArgType::kUnknown;
}

// This function takes the offset of the first member and the size of the second member, and calculates the number of
// bytes of padding that should be inserted between them. It does this by finding the next multiple of the size that is
// greater than or equal to the offset, and then subtracting the offset from that value.
inline constexpr uint32_t CalculatePadding(uint32_t offset, uint32_t size)
{
    // Calculate the next multiple of size that is greater than or equal to offset.
    return ((offset + size - 1) / size * size) - offset;
}

// Usage: Push all your values, then call Finish, afterwards you can pop.
// Example:
//
// PluginAPI::ActionStack stack(2);
// stack.Push(true);
// stack.Push(true);
// stack.Push(1337ull);
// stack.PushString("Hello World!");
// stack.Push(true);
//
// stack.Finish();
// auto v1 = stack.PopBool();
// auto v2 = stack.PopBool();
// auto v3 = stack.PopI64();
//
// String result;
// stack.PopString(result);
//
// auto v4 = stack.PopBool();

class ActionStack
{
public:
    ActionStack(const size_t aArgCount) : m_maxItemCount(static_cast<uint8_t>(aArgCount))
    {
        // assert(aArgCount <= 255);

        // dont bother setting up a stack.
        if (aArgCount == 0)
            return;

        m_pTypeInfo = (ArgType*)malloc(aArgCount * sizeof(ArgType));

        m_pStackBuffer = (uint8_t*)malloc(1024);
        memset(m_pTypeInfo, 0, aArgCount * sizeof(ArgType));
        memset(m_pStackBuffer, 0, 1024);
    }

    ~ActionStack()
    {
        if (m_pTypeInfo)
            free(m_pTypeInfo);
        if (m_pStackBuffer)
            free(m_pStackBuffer);
    }

    inline ArgType GetArgType(uint32_t aIndex) const noexcept
    {
        if (aIndex > m_itemCount || aIndex < 0)
        {
            return ArgType::kUnknown;
        }
        return m_pTypeInfo[aIndex];
    }

    // for user facing code, always use this.
    template <typename... Ts> inline void PushN(const Ts... args)
    {
        (PushScalar(ToArgType<Ts>(), args), ...);
    }

    template <typename... Ts> inline void PopN(Ts&... args)
    {
        (PopScalar(ToArgType<Ts>(), args), ...);
    }

    // this are mainly for internal use, but can be used for user facing code if you know what you're doing.
    inline void Push(bool b) { PushScalar(ArgType::kBool, b); }
    inline bool PopBool() { return PopScalar<bool>(); }

    inline void Push(float f) { PushScalar(ArgType::kF32, f); }
    inline float PopF32() { return PopScalar<float>(); }

    inline void Push(uint32_t i) { PushScalar(ArgType::kU32, i); }
    inline uint32_t PopU32() { return PopScalar<uint32_t>(); }

    inline void Push(uint64_t i) { PushScalar(ArgType::kU64, i); }
    inline uint64_t PopU64() { return PopScalar<uint64_t>(); }

    inline void Push(int32_t i) { PushScalar(ArgType::KI32, i); }
    inline int32_t PopI32() { return PopScalar<int32_t>(); }

    inline void Push(int64_t i) { PushScalar(ArgType::kI64, i); }
    inline int64_t PopI64() { return PopScalar<int64_t>(); }

    SERVER_API_CXX void PushString(const StringRef acString);

    // TODO: string is an unstable interfac.e..
    SERVER_API_CXX void PopString(TiltedPhoques::String& aOutString);

    inline uint32_t GetCount() const
    {
        return m_itemCount;
    }

    // end clears the offset, so you can start popping, but doesn't empty the buffer
    inline void End()
    {
        m_itemOffset = 0;
    }

    inline void Clear()
    {
        std::memset(m_pTypeInfo, 0, m_maxItemCount * sizeof(ArgType));
        std::memset(m_pStackBuffer, 0, 1024);
    }

    auto GetOffset()
    {
        return m_itemOffset;
    }

private:
    template <typename T> constexpr void PushScalar(const ArgType aType, const T acValue)
    {
        m_pTypeInfo[m_itemCount] = aType;
        *reinterpret_cast<T*>(&m_pStackBuffer[m_itemOffset]) = acValue;

        m_itemOffset += sizeof(T);
        m_itemOffset += CalculatePadding(m_itemOffset, sizeof(T));

        m_itemCount++;
    }

    template <typename T> T PopScalar()
    {
        T value = *reinterpret_cast<T*>(&m_pStackBuffer[m_itemOffset]);
        m_itemOffset += sizeof(T);
        m_itemOffset += CalculatePadding(m_itemOffset, sizeof(T));

        m_itemCount--;
        return value;
    }

protected:
    uint8_t* m_pStackBuffer{nullptr};
    ArgType* m_pTypeInfo{nullptr};
    uint8_t m_itemCount{};
    uint8_t m_maxItemCount{};
    uint16_t m_itemOffset{};
};

using MethodHandler = void (*)(ActionStack& acContext);
} // namespace PluginAPI
