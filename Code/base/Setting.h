// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <TiltedCore/Stl.hpp>
#include <Base/Check.h>

namespace base
{
using namespace TiltedPhoques;

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

    enum class Flags : uint16_t
    {
        kNone,
        // Doesn't show up in the help list.
        kHidden = 1 << 0,
        // Value is write protected
        kLocked = 1 << 1,
    };

    SettingBase(SettingBase*& parent) : next(parent)
    {
        parent = this;
    }

    SettingBase() : SettingBase(ROOT())
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
        if constexpr (std::is_same_v<T, char*>)
            return Type::kString;

        return Type::kNone;
    }

    const char* c_str() const
    {
        BASE_ASSERT(type == Type::kString, "Must be a string");
        return data.as_string;
    }

    // type info
    Flags flags{Flags::kNone};
    Type type{Type::kNone};

    // descriptor
    const char* name{nullptr};
    const char* desc{nullptr};
    // Gets aligned to 8 bytes anyway
    size_t dataLength;
    union {
        bool as_boolean;
        int32_t as_int32;
        int64_t as_int64;
        uint32_t as_uint32;
        uint64_t as_uint64;
        float as_float;
        const char* as_string;
    } data{};

  private:
    SettingBase* next;
};

// TODO: this would really benefit from CXX 20 requires keyword
template <typename T> struct Setting : SettingBase
{
    // This could deserve a constinit
    Setting(const char* acName, const char* acDesc, const T acDefault)
    {
        SettingBase::name = acName;
        SettingBase::desc = acDesc;

        if constexpr (std::is_pointer<T>())
            SettingBase::dataLength = std::strlen(acDefault);
        else
            SettingBase::dataLength = sizeof(Type);

        SettingBase::type = ToTypeIndex<T>();
        data.as_int64 = 0;

        // poor mans bit cast
        std::memcpy(&SettingBase::data.as_int64, &acDefault, sizeof(T));
    }

    T& value()
    {
        // TODO: bitcast?
        return reinterpret_cast<T&>(data.as_uint64);
    }

    template <typename Tas> Tas value_as()
    {
        return static_cast<Tas>(data.as_uint64);
    }

    template <typename = typename std::enable_if<true>::type> operator bool()
    {
        return data.as_boolean;
    }
};

} // namespace base
