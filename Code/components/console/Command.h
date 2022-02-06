// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <charconv>
#include <functional>
#include <string>
#include <console/ArgStack.h>

namespace Console
{
// CommandNode
// CommandData


class CommandBase
{
    friend class ConsoleRegistry;

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
    //
    // TODO: does it make sense to even accept any string...
    // TODO: make concept.
    //template <class _Ty>
    //static constexpr bool IsStringType = _Is_any_of_v<remove_cv_t<_Ty>, const char*, std::string>;

    template <class _Ty, class... _Types>
    static constexpr bool IsAnyOf = // true if and only if _Ty is in _Types
        std::disjunction_v<std::is_same<_Ty, _Types>...>;

    template <typename T> static constexpr Type ToValueTypeIndex()
    {
        if constexpr (std::is_same_v<T, bool>)
            return Type::kBoolean;
        if constexpr (std::is_integral_v<T>)
            return Type::kNumeric;
        if constexpr (IsAnyOf<std::remove_cv_t<T>, const char*, std::string>)
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
    std::function<void(ArgStack&)> m_Handler;

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
