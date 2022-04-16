// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <algorithm>
#include <optional>
#include <console/ConsoleRegistry.h>
#include <console/ConsoleUtils.h>
#include <console/StringTokenizer.h>

namespace Console
{
namespace
{
constexpr char kCommandPrefix = '/';

std::unique_ptr<std::string[]> SplitLineTokens(const std::string& arLine, size_t& tokenCount)
{
    if (!CheckIsValidUTF8(arLine))
        return nullptr;

    StringTokenizer tokenizer(&arLine[1]);
    tokenCount = tokenizer.CountTokens();

    auto tokens = std::make_unique<std::string[]>(tokenCount);

    size_t i = 0;
    while (tokenizer.HasMore())
    {
        if (i > tokenCount)
        {
            spdlog::error("Expected {} tokens, got more than {} tokens", tokenCount, i);
            return nullptr;
        }

        tokenizer.GetNext(tokens[i]);
        i++;
    }

    return tokens;
}

std::optional<bool> BoolifyString(std::string_view view)
{
    // let the compiler optimize this.
    if (view == "true" || view == "TRUE" || view == "1")
        return true;
    else if (view == "false" || view == "FALSE" || view == "0")
        return false;
    return std::nullopt;
}
} // namespace

ConsoleRegistry::ConsoleRegistry(const char* acLoggerName)
{
    m_out = spdlog::get(acLoggerName);
    BASE_ASSERT(m_out.get(), "Output logger not found");

    auto* i = CommandBase::ROOT();
    CommandBase::ROOT() = nullptr;
    while (i)
    {
        m_commands.push_back(i);
        auto* j = i->next;
        i->next = nullptr;
        i = j;
    }

    auto* k = SettingBase::ROOT();
    SettingBase::ROOT() = nullptr;
    while (k)
    {
        m_settings.push_back(k);
        auto* j = k->next;
        k->next = nullptr;
        k = j;
    }

    RegisterNatives();
}

ConsoleRegistry::~ConsoleRegistry()
{
    for (CommandBase* c : m_ownedCommands)
        delete c;
    for (SettingBase* s : m_ownedSettings)
        delete s;
}

void ConsoleRegistry::RegisterNatives()
{
    RegisterCommand<>("help", "Show a list of commands", [&](const ArgStack&) {
        m_out->info("<------Commands-({})--->", m_commands.size());
        for (CommandBase* c : m_commands)
        {
            m_out->info("/{}:  {}", c->m_name, c->m_desc);
        }
        m_out->info("<------Variables-({})--->", m_settings.size());
        for (SettingBase* s : m_settings)
        {
            if (!s->IsHidden())
                m_out->info("{}:  {}", s->name, s->desc);
        }
    });

    RegisterCommand<const char*, const char*>(
        "set", R"(Set a setting e.g "set mysetting true")", [&](ArgStack& aStack) {
            const std::string variableName{aStack.Pop<std::string>()};
            if (variableName.empty())
            {
                m_out->error("set <varname> <state>");
                return;
            }

            if (auto* pSetting = FindSetting(variableName.c_str()))
            {
                if (pSetting->IsLocked())
                {
                    m_out->error("Failed apply value: setting is locked");
                    return;
                }

                const auto value{aStack.Pop<std::string>()};
                switch (pSetting->type)
                {
                case SettingBase::Type::kBoolean: {
                    auto result = BoolifyString(value);
                    if (result.has_value())
                        pSetting->data.as_boolean = *result;
                    break;
                }
                case SettingBase::Type::kInt:
                    pSetting->data.as_int32 = ConvertStringValue(value.c_str(), pSetting->data.as_int32);
                    break;
                case SettingBase::Type::kUInt:
                    pSetting->data.as_uint32 = ConvertStringValue(value.c_str(), pSetting->data.as_uint32);
                    break;
                case SettingBase::Type::kInt64:
                    pSetting->data.as_int64 = ConvertStringValue(value.c_str(), pSetting->data.as_int64);
                    break;
                case SettingBase::Type::kUInt64:
                    pSetting->data.as_uint64 = ConvertStringValue(value.c_str(), pSetting->data.as_uint64);
                    break;
                case SettingBase::Type::kFloat:
                    pSetting->data.as_float = ConvertStringValue(value.c_str(), pSetting->data.as_float);
                    break;
                case SettingBase::Type::kString: {
                    static_cast<StringSetting*>(pSetting)->StoreValue(*pSetting, value.c_str());
                    break;
                }
                }

                m_out->info("Set {} to {}", variableName, value);
                return;
            }

            m_out->error("Failed to find setting {}", variableName);
        });
}

void ConsoleRegistry::AddCommand(CommandBase* apCommand)
{
    std::lock_guard<std::mutex> _(m_listLock);
    (void)_;

    // Add to global and tracking pool
    m_commands.push_back(apCommand);
    m_ownedCommands.push_back(apCommand);
}

void ConsoleRegistry::AddSetting(SettingBase* apSetting)
{
    std::lock_guard<std::mutex> guard(m_listLock);
    (void)guard;

    m_settings.push_back(apSetting);
    m_ownedSettings.push_back(apSetting);
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

SettingBase* ConsoleRegistry::FindSetting(const char* acName)
{
    // TODO: Maybe lookup by some hash...
    auto it = std::find_if(m_settings.begin(), m_settings.end(),
                           [&](SettingBase* apSetting) { return std::strcmp(apSetting->name, acName) == 0; });
    if (it == m_settings.end())
        return nullptr;
    return *it;
}

// NOTE(Force): Maybe make this return a status instead?
bool ConsoleRegistry::TryExecuteCommand(const std::string& acLine)
{
    if (acLine.length() <= 2 || acLine[0] != kCommandPrefix)
    {
        m_out->error("Commands must begin with /");
        return false;
    }

    size_t tokenCount = 0;
    auto tokens = SplitLineTokens(acLine, tokenCount);
    if (!tokenCount)
    {
        m_out->error("Failed to parse line");
        return false;
    }

    auto* pCommand = FindCommand(tokens[0].c_str());
    if (!pCommand)
    {
        m_out->error("Unknown command. Type /help for help.");
        return false;
    }

    // Must subtract one, since the first is the literal command.
    tokenCount -= 1;

    if (tokenCount != pCommand->m_argCount)
    {
        m_out->error("Expected {} arguments but got {}", pCommand->m_argCount, tokenCount);
        return false;
    }

    ArgStack stack(pCommand->m_argCount);
    auto result = CreateArgStack(pCommand, &tokens[1], stack);
    if (!result.val)
    {
        m_out->error(result.msg);
        return false;
    }

    stack.ResetCounter();
    m_queue.Upload(pCommand, stack);
    StoreCommandInHistory(acLine);

    return true;
}

void ConsoleRegistry::StoreCommandInHistory(const std::string& acLine)
{
    m_commandHistory.push_back(acLine);

    // Do some housekeeping.
    if (m_commandHistory.size() == 10)
    {
        m_commandHistory.erase(m_commandHistory.end());
    }
}

ResultAnd<bool> ConsoleRegistry::CreateArgStack(const CommandBase* apCommand, const std::string* acStringArgs,
                                                ArgStack& aStackOut)
{
    CommandBase::Type* pType = apCommand->m_pArgIndicesArray;
    for (size_t i = 0; i < apCommand->m_argCount; i++)
    {
        auto& stringArg = acStringArgs[i];
        switch (*pType)
        {
        case CommandBase::Type::kBoolean: {
            auto result = BoolifyString(stringArg);
            if (result.has_value())
            {
                aStackOut.Push(*result /*to boolean conv*/);
                continue;
            }
            return ResultAnd("Expected boolean argument", false);
        }
        case CommandBase::Type::kNumeric: {
            if (!IsNumber(stringArg))
            {
                return ResultAnd("Expected argument of type numeric", false);
            }

            int64_t value = false;
            auto result = std::from_chars(stringArg.data(), stringArg.data() + stringArg.length(), value);
            if (result.ec != std::errc())
            {
                return ResultAnd("Malformed numeric argument", false);
            }

            aStackOut.Push(value);
            break;
        }
        case CommandBase::Type::kString:
            aStackOut.Push(stringArg);
            break;
        default:
            return ResultAnd("Couldn't handle value type", false);
        }
        pType++;
    }

    return ResultAnd(nullptr, true);
}

bool ConsoleRegistry::Update()
{
    // Drain the queue one by one
    if (m_queue.HasWork())
    {
        CommandQueue::Item item;
        m_queue.Fetch(item);
        item.m_pCommand->m_Handler(item.m_stack);
        return true;
    }

    return false;
}
} // namespace Console
