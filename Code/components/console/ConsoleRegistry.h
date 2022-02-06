// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <spdlog/spdlog-inl.h>
#include <console/Command.h>
#include <console/Setting.h>
#include <console/CommandQueue.h>

namespace Console
{

template <typename T> struct ErrorOr
{
    const char* msg;
    T val;
};

template <typename T> struct ResultAnd
{
    explicit ResultAnd(T x) : val(x)
    {
    }

    ResultAnd(const char* msg, T x) : val(x), msg(msg)
    {
    }

    const char* msg{nullptr};
    T val;
};

// Never make this class static
class ConsoleRegistry
{
  public:
    ConsoleRegistry(const char *acLoggerName);
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

    void TryExecuteCommand(const std::string& acLine);

    CommandBase* FindCommand(const char* acName);
    SettingBase* FindSetting(const char* acName);

    // Call this from your main thread, this will drain the work item queue.
    bool Update();

  private:
    void AddCommand(CommandBase* apCommand);
    void AddSetting(SettingBase* apSetting);

    ResultAnd<bool> CreateArgStack(const CommandBase* apCommand, const std::string* acStringArgs, ArgStack& aStackOut);

  private:
    std::mutex m_listLock;
    std::vector<CommandBase*> m_commands;
    std::vector<CommandBase*> m_ownedCommands;
    std::vector<SettingBase*> m_settings;
    std::vector<SettingBase*> m_ownedSettings;
    CommandQueue m_queue;

    std::shared_ptr<spdlog::logger> m_out;
};
} // namespace Console
