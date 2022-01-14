// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>
#include <functional>
#include <any>
#include <charconv>
#include <vector>

namespace base
{
    // CommandNode
    // CommandData

using ArgStack = std::vector<std::any>;

class CommandBase
{
    friend class CommandRegistry;

  public:
    enum class Type
    {
        kBoolean,
        kNumeric,
        kPointer,
        kVoid

        // I think we should introduce signed_numeric
        // and unsigned numeric
    };

    CommandBase(CommandBase*& parent, const char* n, const char* d, size_t argc)
        : m_name(n), m_desc(d), m_argCount(argc), next(parent)
    {
        parent = this;
    }

    explicit CommandBase(const char* n, const char* d, size_t argc) : CommandBase(ROOT(), n, d, argc)
    {
    }

    //static constexpr bool IsIntegral = std::is_integral_v<bool>;

  protected:
    template <typename T> static constexpr Type ToValueTypeIndex()
    {
        if constexpr (std::is_same_v<T, bool>)
            return Type::kBoolean;
        if constexpr (std::is_integral_v<T>)
            return Type::kNumeric;
        if constexpr (std::is_pointer_v<T>)
            static_assert(true, "What are you doing?");
        return Type::kVoid;
    }

  private:
    static inline CommandBase*& ROOT() noexcept
    {
        static CommandBase* root{nullptr};
        return root;
    }

    void Invoke()
    {
    }

  public:
    const char* m_name;
    const char* m_desc;

    // Type info
    const size_t m_argCount;
    Type* m_pArgIndicesArray = nullptr;
    std::function<void(const ArgStack&)> m_Handler;

    // Next static node.
    CommandBase* next = nullptr;
};

template <typename... Ts> class Command : public CommandBase
{
    static constexpr size_t N = sizeof...(Ts);

  public:
    // Construct the command and fill the indices list at runtime.
      // and no, it couldnd be done with std::function
    template<typename TFunctor>
    explicit Command(const char* acName, const char* acDesc, const TFunctor& functor)
        : m_indices{(ToValueTypeIndex<Ts>())...},
        CommandBase(acName, acDesc, N)
    {
        CommandBase::m_pArgIndicesArray = reinterpret_cast<Type*>(&m_indices);
        CommandBase::m_Handler = functor;
    }

    constexpr size_t arg_count()
    {
        return N;
    }

  private:
    Type m_indices[N];
};
} // namespace base
