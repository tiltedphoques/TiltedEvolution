// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <base/Command.h>

namespace base
{

template<typename T>
struct ErrorOr
{
    const char* msg;
    T val;
};

template<typename T>
struct ErrorAnd
{
    explicit ErrorAnd(T x) : val(x)
    {
    }

    ErrorAnd(const char* msg, T x) : val(x), msg(msg)
    {
    }

    const char* msg{nullptr};
    T val;
};

// Never make this class static
class CommandRegistry
{
public:
    CommandRegistry();
    ~CommandRegistry();

    void RegisterInbuiltCommands();

    template <typename... Ts>
    void RegisterCommand(const char* name, const char* desc, std::function<void(ArgStack&)> func)
    {
        // Resources get free`d in the dtor.
        auto pCommand = new Command<Ts...>(name, desc, func);
        m_commands.push_back(pCommand);
        m_ownedCommands.push_back(pCommand);
    }

    CommandBase* FindCommand(const char* acName);

    ErrorAnd<bool> TryExecuteCommand(const char* acName, const std::vector<std::string>& acArgs);

  private:
    std::vector<CommandBase*> m_commands;
    std::vector<CommandBase*> m_ownedCommands;

    // MPSC queue
};
} // namespace base
