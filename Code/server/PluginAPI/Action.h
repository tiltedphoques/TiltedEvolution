// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <PluginAPI/Slice.h>
#include <PluginAPI/APIBoundary.h>

namespace PluginAPI
{
enum class ArgType
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

// This function takes the offset of the first member and the size of the second member, and calculates the number of bytes of padding that should be inserted between them. It does this by finding the next multiple of the size that is greater than or equal to the offset, and then subtracting the
// offset from that value.
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
    ActionStack(const size_t aArgCount)
    {
        if (aArgCount == 0)
        {
            m_pArgTypeStack = nullptr;
            m_pArgStack = nullptr;
            m_argofs = 0;
            return;
        }

        m_pArgTypeStack = (ArgType*)malloc(aArgCount * sizeof(ArgType));

        m_pArgStack = (uint8_t*)malloc(1024);
        memset(m_pArgTypeStack, 0, aArgCount * sizeof(ArgType));
        memset(m_pArgStack, 0, 1024);
        m_argofs = 0;
    }

    ~ActionStack()
    {
        if (m_pArgTypeStack)
            free(m_pArgTypeStack);
        if (m_pArgStack)
            free(m_pArgStack);
    }

    inline ArgType Type() const { return m_pArgTypeStack[m_ArgCount]; }

    inline ArgType GetArgType(uint32_t aIndex) const noexcept
    {
        if (aIndex > m_ArgCount || aIndex < 0)
        {
            return ArgType::kUnknown;
        }
        return m_pArgTypeStack[aIndex];
    }

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

    inline uint32_t count() const { return m_ArgCount; }
    inline void Finish() { m_argofs = 0; }

    auto GetOffset() { return m_argofs; }

private:
    template <typename T> constexpr void PushScalar(const ArgType aType, const T acValue)
    {
        m_pArgTypeStack[m_ArgCount] = aType;
        *reinterpret_cast<T*>(&m_pArgStack[m_argofs]) = acValue;

        m_argofs += sizeof(T);
        m_argofs += CalculatePadding(m_argofs, sizeof(T));

        m_ArgCount++;
    }

    template <typename T> T PopScalar()
    {
        T value = *reinterpret_cast<T*>(&m_pArgStack[m_argofs]);
        m_argofs += sizeof(T);
        m_argofs += CalculatePadding(m_argofs, sizeof(T));

        m_ArgCount--;
        return value;
    }

protected:
    uint8_t* m_pReturnValues{nullptr};
    uint8_t* m_pArgStack{nullptr};
    ArgType* m_pArgTypeStack{nullptr};
    uint32_t m_ArgCount{0};
    uint32_t m_argofs{0};
};

using MethodHandler = void (*)(ActionStack& acContext);
} // namespace PluginAPI
