// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <any>
#include <charconv>
#include <functional>
#include <string>
#include <vector>

namespace base
{
// CommandNode
// CommandData

class ArgStack
{
  public:
    ArgStack() = default;

    template<typename T> void Push(T &&x)
    {
        m_args.push_back(x);
    }

    template <typename T>
    T Pop()
    {
        auto v = std::any_cast<T>(m_args[m_pushCount]);
        m_pushCount++;
        return v;
    }

    void Reset()
    {
        m_args.clear();
        m_pushCount = 0;
    }

  private:
    std::vector<std::any> m_args;
    size_t m_pushCount = 0;
};

class CommandBase
{
    friend class CommandRegistry;

  public:
    enum class Type
    {
        kBoolean,
        kNumeric,
        kString,
        kUnknown

        // I think we should introduce signed_numeric
        // and unsigned numeric
    };

    // TODO: arguments.
    enum class Flags
    {
        kAdminOnly,
        KUserCreated,
    };

    CommandBase(CommandBase*& parent, const char* n, const char* d, size_t argc)
        : m_name(n), m_desc(d), m_argCount(argc), next(parent)
    {
        parent = this;
    }

    explicit CommandBase(const char* n, const char* d, size_t argc) : CommandBase(ROOT(), n, d, argc)
    {
    }

  protected:
    template <class _Ty>
    // TODO: does it make sense to even accept any string...
    static constexpr bool IsStringType = _Is_any_of_v<remove_cv_t<_Ty>, const char*, std::string>;

    template <typename T> static constexpr Type ToValueTypeIndex()
    {
        if constexpr (std::is_same_v<T, bool>)
            return Type::kBoolean;
        if constexpr (std::is_integral_v<T>)
            return Type::kNumeric;
        if constexpr (IsStringType)
            return Type::kString;
        // static_assert(false, "Unsupported type.");
        return Type::kUnknown;
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

// We must specify the expected argument types through the template to be able
// to do proper error checking
template <typename... Ts> class Command : public CommandBase
{
    static constexpr size_t N = sizeof...(Ts);

  public:
    // Construct the command and fill the indices list at runtime.
    // and no, it couldnd be done with std::function
    template <typename TFunctor>
    explicit Command(const char* acName, const char* acDesc, const TFunctor& functor)
        : m_indices{(ToValueTypeIndex<Ts>())..., Type::kUnknown}, CommandBase(acName, acDesc, N)
    {
        CommandBase::m_pArgIndicesArray = reinterpret_cast<Type*>(&m_indices);
        CommandBase::m_Handler = functor;
    }

    constexpr size_t arg_count()
    {
        return N;
    }

  private:
    // At the end we store a stop gap type that should never be hit.
    // We also do this to avoid a potential array of size null
    Type m_indices[N + 1];
};
} // namespace base
