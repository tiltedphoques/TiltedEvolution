// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include "PluginAPI/PluginAPI.h"

namespace PluginAPI
{
enum class ArgType
{
    kBool,
    kU8,
    kI8,
    kU16,
    kI16,
    kU32,
    KI32,
    kU64,
    kI64,
    kF32
};

class ActionStack
{
  public:
    ActionStack(uint32_t count)
    {
        m_pArgTypeStack = (ArgType*)malloc(count * sizeof(ArgType));
        m_pArgStack = (uint8_t*)malloc(1024);
        memset(m_pArgTypeStack, 0, count * sizeof(ArgType));
        memset(m_pArgStack, 0, 1024);
        m_argofs = 0;
    }

    ~ActionStack()
    {
        free(m_pArgTypeStack);
        free(m_pArgStack);
    }

    ArgType Type() const
    {
        return m_pArgTypeStack[m_ArgCount];
    }

    ArgType GetArg(int i) const
    {
        return m_pArgTypeStack[i];
    }

    void Push(bool b)
    {
        PushVal(ArgType::kBool, b);
    }
    void Push(float f)
    {
        PushVal(ArgType::kF32, f);
    }
    // unsigned
    void Push(uint8_t i)
    {
        PushVal(ArgType::kU8, i);
    }
    void Push(uint16_t i)
    {
        PushVal(ArgType::kU16, i);
    }
    void Push(uint32_t i)
    {
        PushVal(ArgType::kU32, i);
    }
    void Push(uint64_t i)
    {
        PushVal(ArgType::kU64, i);
    }
    // signed
    void Push(int8_t i)
    {
        PushVal(ArgType::kI8, i);
    }
    void Push(int16_t i)
    {
        PushVal(ArgType::kI16, i);
    }
    void Push(int32_t i)
    {
        PushVal(ArgType::KI32, i);
    }
    void Push(int64_t i)
    {
        PushVal(ArgType::kI64, i);
    }

    bool PopBool()
    {
        return FetchVal<bool>();
    }
    float PopF32()
    {
        return FetchVal<float>();
    }
    // unsigned
    bool PopU8()
    {
        return FetchVal<uint8_t>();
    }
    bool PopU16()
    {
        return FetchVal<uint16_t>();
    }
    bool PopU32()
    {
        return FetchVal<uint32_t>();
    }
    bool PopU64()
    {
        return FetchVal<uint64_t>();
    }
    // signed
    bool PopI8()
    {
        return FetchVal<int8_t>();
    }
    bool PopI16()
    {
        return FetchVal<int16_t>();
    }
    bool PopI32()
    {
        return FetchVal<int32_t>();
    }
    bool PopI64()
    {
        return FetchVal<int64_t>();
    }

    inline uint32_t count() const
    {
        return m_ArgCount;
    }

    void End()
    {
        m_argofs = 0;
    }

  private:
    template <typename T> void PushVal(const ArgType aType, const T acValue)
    {
        m_pArgTypeStack[m_ArgCount] = aType;
        *reinterpret_cast<T*>(&m_pArgStack[m_argofs]) = acValue;
        m_argofs += sizeof(T);
        m_ArgCount++;
    }

    template <typename T> T FetchVal()
    {
        T value = *reinterpret_cast<T*>(&m_pArgStack[m_argofs]);
        m_argofs += sizeof(T);
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

using ActionCallback = void (*)(ActionStack& acContext);
} // namespace PluginAPI    
