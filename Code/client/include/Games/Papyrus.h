#pragma once

#include <Games/Skyrim/Misc/SkyrimVM.h>
#include <Games/Skyrim/Misc/BSScript.h>

#include <Games/Fallout4/Misc/GameVM.h>
#include <Games/Fallout4/Misc/BSScript.h>

#include <tuple>

template<class... T>
struct EventArguments : BSScript::IFunctionArguments
{
    using Tuple = std::tuple<EventArguments...>;

    EventArguments(T... args) : args(std::forward<T>(args)...) {}
    virtual ~EventArguments(){};

    virtual void Prepare(BSScript::IFunctionArguments::Statement* apStatement) noexcept
    {
        apStatement->SetSize(std::tuple_size_v<std::remove_reference_t<Tuple>>);

        PrepareImplementation(apStatement,
                              std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }

private:
     
    template <std::size_t... Is> 
    void PrepareImplementation(BSScript::IFunctionArguments::Statement* apStatement, std::index_sequence<Is...>) noexcept
    {
        ((apStatement->values[Is].Set(std::get<Is>(args)), ...);
    }

    Tuple args;
};
