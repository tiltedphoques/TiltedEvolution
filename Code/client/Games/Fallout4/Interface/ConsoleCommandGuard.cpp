#include <TiltedOnlinePCH.h>

#include <World.h>

// Refuses typed console commands while connected to a server with Gameplay:bAllowConsoleCommands off.
// Commands run by code (Console::ExecuteCommand, 2248537) don't go through this callback.
//
// 2248536 is Console::Call, the Scaleform callback for "executeCommand" (user data 0) and "onHideComplete" (1).
// 2248591 is ConsoleLog::PrintLine, 4797437 the ConsoleLog singleton.

namespace
{
// Same list as Skyrim, see its ConsoleCommandGuard.cpp
constexpr std::string_view kAllowedCommands[]{
    "tfc", "ToggleFreeCamera",
    "tm",  "ToggleMenus",
    "coc", "CenterOnCell",
};

// Scaleform::GFx::Value and FunctionHandler::Params, from the 1.10.155 debug build types
struct GFxValue
{
    void* pObjectInterface; // 00
    uint32_t type;          // 08
    uint32_t pad0C;         // 0C
    void* pValue;           // 10
    uint64_t dataAux;       // 18
};
static_assert(sizeof(GFxValue) == 0x20);

struct GFxFunctionParams
{
    GFxValue* pRetVal;          // 00
    void* pMovie;               // 08
    GFxValue* pThis;            // 10
    GFxValue* pArgsWithThisRef; // 18
    GFxValue* pArgs;            // 20
    uint32_t argCount;          // 28
    void* pUserData;            // 30
};
static_assert(offsetof(GFxFunctionParams, argCount) == 0x28);
static_assert(offsetof(GFxFunctionParams, pUserData) == 0x30);

constexpr uint32_t kGFxString = 6;
constexpr uint32_t kGFxManagedBit = 0x40;
constexpr uint32_t kGFxTypeMask = 0x8F;

bool EqualsIgnoreCase(std::string_view aLhs, std::string_view aRhs) noexcept
{
    return aLhs.size() == aRhs.size() && _strnicmp(aLhs.data(), aRhs.data(), aLhs.size()) == 0;
}

bool IsSpace(char aChar) noexcept
{
    return aChar == ' ' || aChar == '\t';
}

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

const char* GetTypedLine(const GFxFunctionParams* apParams) noexcept
{
    if (!apParams || apParams->pUserData || apParams->argCount != 1 || !apParams->pArgs)
        return nullptr;

    const GFxValue& cValue = apParams->pArgs[0];
    if ((cValue.type & kGFxTypeMask) != kGFxString || !cValue.pValue)
        return nullptr;

    return (cValue.type & kGFxManagedBit) ? *static_cast<const char**>(cValue.pValue) : static_cast<const char*>(cValue.pValue);
}

void PrintToConsole(const char* acpText) noexcept
{
    using TPrintLine = void(void*, const char*, ...);
    static VersionDbPtr<TPrintLine> s_printLine(2248591);
    static VersionDbPtr<void*> s_consoleLog(4797437);

    void* pConsoleLog = *s_consoleLog.Get();
    if (pConsoleLog)
        s_printLine.Get()(pConsoleLog, "%s", acpText);
}

bool AreTypedCommandsBlocked() noexcept
{
    World& world = World::Get();
    return world.GetTransport().IsConnected() && !world.GetServerSettings().ConsoleCommandsAllowed;
}
} // namespace

using TConsoleCall = void(void* apThis, const GFxFunctionParams* apParams);
static TConsoleCall* RealConsoleCall = nullptr;

static void HookConsoleCall(void* apThis, const GFxFunctionParams* apParams)
{
    const char* cpLine = GetTypedLine(apParams);
    if (!cpLine || !AreTypedCommandsBlocked() || IsAllowedCommand(cpLine))
    {
        RealConsoleCall(apThis, apParams);
        return;
    }

    spdlog::info("Console command refused, the server has turned console commands off: {}", cpLine);
    PrintToConsole("Console commands are turned off on this server.");
}

static TiltedPhoques::Initializer s_consoleCommandGuardHooks(
    []()
    {
        VersionDbPtr<TConsoleCall> s_consoleCall(2248536);
        RealConsoleCall = s_consoleCall.Get();
        TP_HOOK(&RealConsoleCall, HookConsoleCall);
    });
