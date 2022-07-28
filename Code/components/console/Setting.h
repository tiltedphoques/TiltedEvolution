// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <TiltedCore/Stl.hpp>
#include <base/Check.h>

namespace Console
{
using namespace TiltedPhoques;

enum class SettingsFlags : uint16_t
{
    kNone,
    // Doesn't show up in the help list.
    kHidden = 1 << 0,
    // Value is write protected, cannot be altered
    // at runtime.
    kLocked = 1 << 1,
    // Does alter game-play
    kCheat = 1 << 2,
};

inline constexpr SettingsFlags operator|(SettingsFlags lhs, SettingsFlags rhs)
{
    return static_cast<SettingsFlags>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
}

inline constexpr bool operator&(SettingsFlags lhs, SettingsFlags rhs)
{
    return static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs);
}

struct SettingBase
{
    enum class Type : uint16_t
    {
        kNone,
        kBoolean,
        kInt,
        kInt64,
        kUInt,
        kUInt64,
        kFloat,
        kString
    };

    SettingBase(SettingBase*& parent, const char* n, const char* d, Type t, SettingsFlags f)
        : next(parent), flags(f), type(t), name(n), desc(d)
    {
        parent = this;
    }

    explicit SettingBase(const char* n, const char* d, Type t, SettingsFlags f) : SettingBase(ROOT(), n, d, t, f)
    {
    }

    static inline SettingBase*& ROOT() noexcept
    {
        static SettingBase* root{nullptr};
        return root;
    }

    template <typename TFunc> static void VisitAll(const TFunc& functor)
    {
        for (auto* i = ROOT(); i;)
        {
            functor(i);
            i = i->next;
        }
    }

    template <typename T> static constexpr Type ToTypeIndex()
    {
        if constexpr (std::is_same_v<T, bool>)
            return Type::kBoolean;
        if constexpr (std::is_same_v<T, int32_t>)
            return Type::kInt;
        if constexpr (std::is_same_v<T, int64_t>)
            return Type::kInt64;
        if constexpr (std::is_same_v<T, uint32_t>)
            return Type::kUInt;
        if constexpr (std::is_same_v<T, uint64_t>)
            return Type::kUInt64;
        if constexpr (std::is_same_v<T, float>)
            return Type::kFloat;
        if constexpr (std::is_same_v<T, const char*>)
            return Type::kString;
        return Type::kNone;
    }

    const char* c_str() const
    {
        BASE_ASSERT(type == Type::kString, "Must be a string");
        return data.as_string;
    }

    inline bool IsHidden() const
    {
        return flags & SettingsFlags::kHidden;
    }

    inline bool IsLocked() const
    {
        return flags & SettingsFlags::kLocked;
    }

    inline bool IsCheat() const
    {
        return flags & SettingsFlags::kCheat;
    }

    // type info
    SettingsFlags flags;
    Type type;

    // descriptor
    const char* name;
    const char* desc;

    // Gets aligned to 8 bytes anyway
    size_t dataLength = 0;
    union {
        bool as_boolean;
        int32_t as_int32;
        int64_t as_int64;
        uint32_t as_uint32;
        uint64_t as_uint64;
        float as_float;
        const char* as_string;
    } data{};

    SettingBase* next;
};

namespace detail
{
template <typename T> struct FixedStorage
{
    constexpr FixedStorage(SettingBase& b, const T acValue)
    {
        b.dataLength = sizeof(T);
        b.data.as_int64 = 0;

        StoreValue(b, acValue);
    }

    inline constexpr void StoreValue(SettingBase& b, const T acValue)
    {
        // The Size is never gonna change, so we only need to update the actual
        // data
        std::memcpy(&b.data.as_int64, &acValue, sizeof(T));
    }
};

template <typename T> struct DynamicStringStorage
{
    DynamicStringStorage(SettingBase& b, const T* acValue)
    {
        StoreValue(b, acValue);
    }

    inline void StoreValue(SettingBase& b, const T* acValue)
    {
        m_data = acValue;
        b.dataLength = m_data.length() * sizeof(T);
        b.data.as_string = m_data.c_str();
    }

  private:
    std::basic_string<T, std::char_traits<T>, StlAllocator<T>> m_data;
};
} // namespace detail

// Settings can have their own custom storage spaces.
// However, make sure to make the providers aware of this.
template <typename T, class TStorage = detail::FixedStorage<T>> class Setting : public SettingBase, public TStorage
{
  public:
    Setting(const char* acName, const char* acDesc, const T acDefault, const SettingsFlags acFlags = SettingsFlags::kNone)
        : SettingBase(acName, acDesc, ToTypeIndex<T>(), acFlags), TStorage(*this, acDefault)
    {
    }

    Setting() = delete;

    T value() const
    {
        return reinterpret_cast<T>(data.as_uint64);
    }

    template <typename Tas> Tas value_as() const
    {
        return static_cast<Tas>(data.as_uint64);
    }

    float as_float() const
    {
        return data.as_float;
    }

    operator bool()
    {
        static_assert(std::is_same_v<T, bool>, "Must be a boolean");
        return data.as_boolean;
    }

    bool empty()
    {
        return dataLength == 0;
    }

    void operator=(const T value)
    {
        if (flags & SettingsFlags::kLocked)
        {
            BASE_ASSERT(false, "Tried to write to locked variable");
            return;
        }

        TStorage::StoreValue(*this, value);
    }
};

using StringSetting = Setting<const char*, detail::DynamicStringStorage<char>>;
// NOTE: Wide strings are not supported, since our INI cant handle them.
} // namespace Console
