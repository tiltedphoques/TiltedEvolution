// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <algorithm>
#include <base/CommandRegistry.h>
#include <cctype>

namespace base
{
namespace
{
// SEE
// https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it
bool IsNumber(const std::string& s)
{
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](char c) { return !(std::isdigit(c) || c == '.'); }) == s.end();
}

bool utf8_check_is_valid(const std::string& string)
{
    int c, i, ix, n, j;
    for (i = 0, ix = string.length(); i < ix; i++)
    {
        c = (unsigned char)string[i];
        // if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
        if (0x00 <= c && c <= 0x7f)
            n = 0; // 0bbbbbbb
        else if ((c & 0xE0) == 0xC0)
            n = 1; // 110bbbbb
        else if (c == 0xed && i < (ix - 1) && ((unsigned char)string[i + 1] & 0xa0) == 0xa0)
            return false; // U+d800 to U+dfff
        else if ((c & 0xF0) == 0xE0)
            n = 2; // 1110bbbb
        else if ((c & 0xF8) == 0xF0)
            n = 3; // 11110bbb
        // else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        // else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else
            return false;
        for (j = 0; j < n && i < ix; j++)
        { // n bytes matching 10bbbbbb follow ?
            if ((++i == ix) || (((unsigned char)string[i] & 0xC0) != 0x80))
                return false;
        }
    }
    return true;
}
} // namespace

CommandRegistry::CommandRegistry()
{
    // Register global commands.
    for (auto* i = CommandBase::ROOT(); i;)
    {
        m_commands.push_back(i);
        i = i->next;
    }

    RegisterInbuiltCommands();
}

CommandRegistry::~CommandRegistry()
{
    for (CommandBase* c : m_ownedCommands)
    {
        delete c;
    }
}

void CommandRegistry::RegisterInbuiltCommands()
{
    RegisterCommand<>("help", "Show a list of commands", [&](const base::ArgStack& args) {

    });
}

CommandBase* CommandRegistry::FindCommand(const char* acName)
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
ErrorAnd<bool> CommandRegistry::TryExecuteCommand(const char* acName, const std::vector<std::string>& acArgs)
{
    auto* pCommand = FindCommand(acName);
    if (!pCommand)
        return ErrorAnd("Failed to find command", false);

    if (acArgs.size() != pCommand->m_argCount)
    {
        return ErrorAnd("Expected %d arguments, but x", false);
    }

    ArgStack stack;
    CommandBase::Type* pType = pCommand->m_pArgIndicesArray;
    for (size_t i = 0; i < pCommand->m_argCount; i++)
    {
        auto& stringArg = acArgs[i];
        if (!utf8_check_is_valid(stringArg))
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

    pCommand->m_Handler(stack);

    // Need to queue execution onto the right thread...
    return ErrorAnd(true);
}
} // namespace base
