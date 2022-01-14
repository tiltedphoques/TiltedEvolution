// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <base/Command.h>

namespace base
{
namespace detail
{
// SEE
// https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it
bool IsNumber(const std::string& s)
{
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](char c) { return !(std::isdigit(c) || c == '.'); }) == s.end();
}
} // namespace detail

// Never make this class static
class CommandRegistry
{
  public:
    enum class Status
    {
        kSuccess,
        kNotFound,
        kMissingArgs
    };

    CommandRegistry()
    {
        // Register global commands.
        for (auto* i = CommandBase::ROOT(); i;)
        {
            m_commands.push_back(i);
            i = i->next;
        }
    }

    template <typename... Ts> void RegisterCommand(const char* name, const char* desc, std::function<void()> func)
    {
        auto pCommand = new Command<Ts>(name, desc, func);
        m_commands.push_back(pCommand);
    }

    CommandBase* FindCommand(const char* acName)
    {
        // TODO: Maybe lookup by some hash...
        auto it = std::find_if(m_commands.begin(), m_commands.end(),
                               [&](CommandBase* apCommand) { return std::strcmp(apCommand->m_name, acName) == 0; });
        if (it == m_commands.end())
            return nullptr;
        return *it;
    }

    Status TryExecuteCommand(const char* acName, const std::vector<std::string>& acArgs)
    {
        auto* pCommand = FindCommand(acName);
        if (!pCommand)
            return Status::kNotFound;

        // Validate arg counts against each other.
        if (acArgs.size() != pCommand->m_argCount)
        {
            // Invalid 
            
        }

        ArgStack stack;
        CommandBase::Type* pType = pCommand->m_pArgIndicesArray;
        for (size_t i = 0; i < pCommand->m_argCount; i++)
        {
            auto& stringArg = acArgs[i];
            switch (*pType)
            {
            case CommandBase::Type::kBoolean: {
                if (stringArg == "true" || stringArg == "TRUE")
                {
                    stack.push_back(true);
                    continue;
                }
                else if (stringArg == "false" || stringArg == "FALSE")
                {
                    stack.push_back(false);
                    continue;
                }

                // Expected x got this
                return Status::kMissingArgs;
            }
            case CommandBase::Type::kNumeric: {
                if (!detail::IsNumber(stringArg))
                {
                    // Expected argument of type numeric, got this

                    __debugbreak();
                }

                int64_t value = false;
                auto result = std::from_chars(stringArg.data(), stringArg.data() + stringArg.length(), value);
                if (result.ec != std::errc())
                {
                    __debugbreak();

                    // Expected number, got this, number malformed
                    return Status::kMissingArgs;
                }

                stack.push_back(value);
                break;
            }
            case CommandBase::Type::kVoid:
            case CommandBase::Type::kPointer:
                __debugbreak();
                // Ignore.
                break;
            }
            pType++;
        }

        pCommand->m_Handler(stack);
        // Need to queue execution onto the right thread...
        return Status::kSuccess;
    }

  private:
    std::vector<CommandBase*> m_commands;

    // MPSC queue
};
} // namespace base
