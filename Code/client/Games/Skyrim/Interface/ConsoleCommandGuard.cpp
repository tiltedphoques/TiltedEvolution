#include <TiltedOnlinePCH.h>

#include <World.h>

// Lets a server turn off console commands that players type and send with Enter (Gameplay:bAllowConsoleCommands).
// The console still opens, so clicking a reference to read its id keeps working, and commands that code runs
// (ConsoleUtil, Papyrus ExecuteCommand, the F2 chat /wave) are untouched: none of them go through this callback.
//
// Engine notes, checked in the 1.6.1170 and 1.7.104 databases (same bodies, only the addresses differ):
// - 51083 is the Console menu's FxDelegate Accept; it registers "ExecuteCommand" -> 51084.
// - 51084 is that callback, void(const FxDelegateArgs&). It builds a Script from the typed line (Script::SetText,
//   21883) and queues a task that compiles and runs it later. It echoes nothing and keeps no history itself.
// - 51109 is ConsoleLog::Print(ConsoleLog*, const char* fmt, ...), 401203 the ConsoleLog* singleton.

namespace
{
// Minimal Scaleform layouts used by the callback (CommonLibSSE-NG GFxValue / FxDelegateArgs).
struct GFxValueRaw
{
    void* pObjectInterface; // 00
    uint32_t type;          // 08
    uint32_t pad0C;         // 0C
    void* pValue;           // 10
};
static_assert(sizeof(GFxValueRaw) == 0x18);

struct FxDelegateArgsRaw
{
    GFxValueRaw responseId;    // 00
    void* pHandler;            // 18
    void* pMovie;              // 20
    const GFxValueRaw* pArgs;  // 28
    uint32_t numArgs;          // 30
};
static_assert(offsetof(FxDelegateArgsRaw, pArgs) == 0x28);

constexpr uint32_t kGFxTypeString = 4;
constexpr uint32_t kGFxManagedBit = 0x40;

const char* GetTypedLine(const FxDelegateArgsRaw& acArgs) noexcept
{
    if (!acArgs.pArgs || acArgs.numArgs == 0)
        return nullptr;

    const GFxValueRaw& cValue = acArgs.pArgs[0];
    if ((cValue.type & 0x8F) != kGFxTypeString || !cValue.pValue)
        return nullptr;

    // A managed string holds a pointer to the string's data pointer.
    if (cValue.type & kGFxManagedBit)
        return *static_cast<const char* const*>(cValue.pValue);

    return static_cast<const char*>(cValue.pValue);
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

using TExecuteCommand = void(const FxDelegateArgsRaw&);
static TExecuteCommand* RealExecuteCommand = nullptr;

static void HookExecuteCommand(const FxDelegateArgsRaw& acArgs)
{
    if (!AreTypedCommandsBlocked())
    {
        RealExecuteCommand(acArgs);
        return;
    }

    const char* cpLine = GetTypedLine(acArgs);
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
