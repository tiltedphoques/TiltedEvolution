#include <TiltedOnlinePCH.h>

#include <World.h>
#include <Interface/IMenu.h>

// Refuses typed console commands while connected to a server with Gameplay:bAllowConsoleCommands off.
// Commands run by code (ConsoleUtil, Papyrus ExecuteCommand) don't go through this callback.
//
// 51084 is the Console menu's "ExecuteCommand" callback (registered by 51083). It queues the typed line as a
// Script to be compiled and run later. 51109 is ConsoleLog::Print, 401203 the ConsoleLog singleton.

namespace
{
// Commands that stay allowed while typed commands are turned off, by short and full name. They help players
// get past things that block a playthrough without affecting other players.
//
// tgm and tcl stay blocked: tgm makes a player invulnerable in PvP, and tcl lets a player walk through walls
// and breaks enemy pathing. The game already offers fair alternatives: fast travel, teleporting to a party
// member, and the Become Ethereal shout.
constexpr std::string_view kAllowedCommands[]{
    "tfc", "ToggleFreeCamera",
    "tm",  "ToggleMenus",
    "coc", "CenterOnCell",
};

bool EqualsIgnoreCase(std::string_view aLhs, std::string_view aRhs) noexcept
{
    return aLhs.size() == aRhs.size() && _strnicmp(aLhs.data(), aRhs.data(), aLhs.size()) == 0;
}

bool IsSpace(char aChar) noexcept
{
    return aChar == ' ' || aChar == '\t';
}

// Only the command name is checked. A reference prefix ("player.tcl") isn't allowed, so the command can't be
// aimed at another reference.
bool IsAllowedCommand(std::string_view aLine) noexcept
{
    if (aLine.find_first_of("\r\n") != std::string_view::npos)
        return false;

    while (!aLine.empty() && IsSpace(aLine.front()))
        aLine.remove_prefix(1);

    size_t nameLength = 0;
    while (nameLength < aLine.size() && !IsSpace(aLine[nameLength]))
        ++nameLength;

    const std::string_view cName = aLine.substr(0, nameLength);
    for (const std::string_view cAllowed : kAllowedCommands)
    {
        if (EqualsIgnoreCase(cName, cAllowed))
            return true;
    }

    return false;
}

const char* GetTypedLine(const FxDelegateArgs* apArgs) noexcept
{
    if (!apArgs || !apArgs->pArgs || apArgs->GetArgCount() == 0)
        return nullptr;

    const GFxValue& cValue = (*apArgs)[0];
    if (!cValue.IsString() || !cValue.value.pObject)
        return nullptr;

    return cValue.GetString();
}

void PrintToConsole(const char* acpText) noexcept
{
    using TPrint = void(void*, const char*, ...);
    POINTER_SKYRIMSE(TPrint, s_print, 51109);
    POINTER_SKYRIMSE(void*, s_consoleLog, 401203);

    void* pConsoleLog = *s_consoleLog.Get();
    if (pConsoleLog)
        s_print.Get()(pConsoleLog, "%s", acpText);
}

bool AreTypedCommandsBlocked() noexcept
{
    World& world = World::Get();
    return world.GetTransport().IsConnected() && !world.GetServerSettings().ConsoleCommandsAllowed;
}
} // namespace

using TExecuteCommand = void(const FxDelegateArgs* apArgs);
static TExecuteCommand* RealExecuteCommand = nullptr;

static void HookExecuteCommand(const FxDelegateArgs* apArgs)
{
    if (!AreTypedCommandsBlocked())
    {
        RealExecuteCommand(apArgs);
        return;
    }

    const char* cpLine = GetTypedLine(apArgs);
    if (cpLine && IsAllowedCommand(cpLine))
    {
        RealExecuteCommand(apArgs);
        return;
    }

    spdlog::info("Console command refused, the server has turned console commands off: {}", cpLine ? cpLine : "<none>");
    PrintToConsole("Console commands are turned off on this server.");
}

static TiltedPhoques::Initializer s_consoleCommandGuardHooks(
    []()
    {
        POINTER_SKYRIMSE(TExecuteCommand, s_executeCommand, 51084);
        RealExecuteCommand = s_executeCommand.Get();
        TP_HOOK(&RealExecuteCommand, HookExecuteCommand);
    });
