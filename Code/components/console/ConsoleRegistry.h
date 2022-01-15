// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <base/fast_queue.hpp>
#include <console/Command.h>
#include <console/Setting.h>

namespace console
{

template <typename T> struct ErrorOr
{
    const char* msg;
    T val;
};

template <typename T> struct ErrorAnd
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
class ConsoleRegistry
{
  public:
    ConsoleRegistry();
    ~ConsoleRegistry();

    void RegisterNatives();

    template <typename... Ts>
    inline void RegisterCommand(const char* acName, const char* acDesc, std::function<void(ArgStack&)> func)
    {
        AddCommand(new Command<Ts...>(acName, acDesc, func));
    }

    inline void RegisterSetting(const char* acName, const char* acDesc, const char* acString)
    {
        AddSetting(new StringSetting(acName, acDesc, acString));
    }

    template <typename T> inline void RegisterSetting(const char* acName, const char* acDesc, const T acDefault)
    {
        AddSetting(new Setting<T>(acName, acDesc, acDefault));
    }

    CommandBase* FindCommand(const char* acName);
    ErrorAnd<bool> ScheduleCommand(const char* acName, const std::vector<std::string>& acArgs);

  private:
    void AddCommand(CommandBase* apCommand);
    void AddSetting(SettingBase* apSetting);

  private:
    std::mutex m_listLock;
    std::vector<CommandBase*> m_commands;
    std::vector<CommandBase*> m_ownedCommands;
    std::vector<SettingBase*> m_settings;

    struct CommandItem
    {
        // Strictly speaking, this also isn't thread safe...
        CommandBase* m_pCommand{nullptr};
        ArgStack m_stack;
    };
    fast_queue<CommandItem> m_queue;
};
} // namespace console
