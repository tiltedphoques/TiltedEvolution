// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <console/Command.h>
#include <console/CommandQueue.h>
#include <console/Setting.h>

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
    ConsoleRegistry(const char* acLoggerName);
    ~ConsoleRegistry();

    void RegisterNatives();
    void BindStaticItems();

    template <typename... Ts>
    inline void RegisterCommand(const char* acName, const char* acDesc, std::function<void(ArgStack&)> func)
    {
        AddCommand(
            TiltedPhoques::CastUnique<CommandBase>(TiltedPhoques::MakeUnique<Command<Ts...>>(acName, acDesc, func)));
    }

    inline void RegisterSetting(const char* acName, const char* acDesc, const char* acString)
    {
        AddSetting(
            TiltedPhoques::CastUnique<SettingBase>(TiltedPhoques::MakeUnique<StringSetting>(acName, acDesc, acString)));
    }

    template <typename T> inline void RegisterSetting(const char* acName, const char* acDesc, const T acDefault)
    {
        AddSetting(
            TiltedPhoques::CastUnique<SettingBase>(TiltedPhoques::MakeUnique<Setting<T>>(acName, acDesc, acDefault)));
    }

    enum class ExecutionResult
    {
        kFailure,
        kSuccess,
        kDirty
    };

    ExecutionResult TryExecuteCommand(const TiltedPhoques::String& acLine);

    CommandBase* FindCommand(const char* acName);
    SettingBase* FindSetting(const char* acName);

    // Note that this is not thread safe, call this from the same thread you requested
    // the execution from.
    auto& GetCommandHistory() const noexcept
    {
        return m_commandHistory;
    }

    // Call this from your main thread, this will drain the work item queue.
    bool Update();

    template <typename T> void ForAllCommands(T functor)
    {
        for (auto& c : m_commands)
            functor(c);
    }

    template <typename T> void ForAllSettings(T functor)
    {
        for (auto& c : m_settings)
            functor(c);
    }

    void MarkDirty() noexcept
    {
        m_requestFlush = true;
    }

  private:
    void AddCommand(TiltedPhoques::UniquePtr<CommandBase> apCommand);
    void AddSetting(TiltedPhoques::UniquePtr<SettingBase> apSetting);
    void StoreCommandInHistory(const TiltedPhoques::String& acLine);

    ResultAnd<bool> CreateArgStack(const CommandBase* apCommand, const TiltedPhoques::String* acStringArgs,
                                   ArgStack& aStackOut);

  private:
    std::mutex m_listLock;
    TiltedPhoques::Vector<CommandBase*> m_commands;
    TiltedPhoques::Vector<SettingBase*> m_settings;
    TiltedPhoques::Vector<UniquePtr<CommandBase>> m_dynamicCommands;
    TiltedPhoques::Vector<UniquePtr<SettingBase>> m_dynamicSettings;
    TiltedPhoques::Vector<TiltedPhoques::String> m_commandHistory;
    CommandQueue m_queue;
    bool m_requestFlush = true;

    std::shared_ptr<spdlog::logger> m_out;
};
} // namespace Console
