// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <algorithm>
#include <console/ConsoleRegistry.h>
#include <console/ConsoleUtils.h>

namespace console
{
ConsoleRegistry::ConsoleRegistry()
{
    // Register global stuff.
    for (auto* i = CommandBase::ROOT(); i;)
    {
        m_commands.push_back(i);
        i = i->next;
    }

    for (auto* i = SettingBase::ROOT(); i;)
    {
        m_settings.push_back(i);
        i = i->next;
    }

    RegisterNatives();
}

ConsoleRegistry::~ConsoleRegistry()
{
    for (CommandBase* c : m_ownedCommands)
        delete c;
}

void ConsoleRegistry::RegisterNatives()
{
    // TODO: put this strictly on console out.
    RegisterCommand<>("help", "Show a list of commands", [&](const ArgStack&) {
        spdlog::info("<------Commands-({})--->", m_commands.size());
        for (CommandBase* c : m_commands)
        {
            spdlog::info("/{}:  {}", c->m_name, c->m_desc);
        }
        spdlog::info("<------Variables-({})--->", m_settings.size());
        for (SettingBase* s : m_settings)
        {
            if (!s->IsHidden())
                spdlog::info("{}:  {}", s->name, s->desc);
        }
    });
}

void ConsoleRegistry::AddCommand(CommandBase* apCommand)
{
    std::lock_guard<std::mutex> guard(m_listLock);
    (void)guard;

    // Add to global and tracking pool
    m_commands.push_back(apCommand);
    m_ownedCommands.push_back(apCommand);
}

void ConsoleRegistry::AddSetting(SettingBase* apSetting)
{
    std::lock_guard<std::mutex> guard(m_listLock);
    (void)guard;

    m_settings.push_back(apSetting);
}

CommandBase* ConsoleRegistry::FindCommand(const char* acName)
{
    // TODO: Maybe lookup by some hash...
    auto it = std::find_if(m_commands.begin(), m_commands.end(),
                           [&](CommandBase* apCommand) { return std::strcmp(apCommand->m_name, acName) == 0; });
    if (it == m_commands.end())
        return nullptr;
    return *it;
}

// This is supposed on a custom dedicated thread. When the work is submitted it gets written into the thread queue to be
// consumed By the owning threads.
ErrorAnd<bool> ConsoleRegistry::ScheduleCommand(const char* acName, const std::vector<std::string>& acArgs)
{
    auto* pCommand = FindCommand(acName);
    if (!pCommand)
        return ErrorAnd("Failed to find command", false);

    if (acArgs.size() != pCommand->m_argCount)
    {
        return ErrorAnd("Expected %d arguments, but x", false);
    }

    ArgStack stack(pCommand->m_argCount);
    CommandBase::Type* pType = pCommand->m_pArgIndicesArray;
    for (size_t i = 0; i < pCommand->m_argCount; i++)
    {
        auto& stringArg = acArgs[i];
        if (!CheckIsValidUTF8(stringArg))
        {
            return ErrorAnd("Malformed character sequence (Not UTF8)", false);
        }

        switch (*pType)
        {
        case CommandBase::Type::kBoolean: {
            if (stringArg == "true" || stringArg == "TRUE" || stringArg == "1")
            {
                stack.Push(true);
                continue;
            }
            else if (stringArg == "false" || stringArg == "FALSE" || stringArg == "0")
            {
                stack.Push(false);
                continue;
            }

            return ErrorAnd("Expected boolean argument, got y", false);
        }
        case CommandBase::Type::kNumeric: {
            if (!IsNumber(stringArg))
            {
                return ErrorAnd("Expected argument of type numeric", false);
            }

            int64_t value = false;
            auto result = std::from_chars(stringArg.data(), stringArg.data() + stringArg.length(), value);
            if (result.ec != std::errc())
            {
                return ErrorAnd("Malformed numeric argument", false);
            }

            stack.Push(value);
            break;
        }
        case CommandBase::Type::kString:
            stack.Push(stringArg);
            break;
        default:
            return ErrorAnd("Couldn't handle value of type d", false);
        }
        pType++;
    }

    stack.ResetCounter();

    // Upload Item
    CommandItem* pItem;
    {
        auto lock = m_queue.write_acquire(pItem);
        pItem->m_pCommand = pCommand;
        pItem->m_stack = std::move(stack);
    }
     
    return ErrorAnd(true);
}
} // namespace base
