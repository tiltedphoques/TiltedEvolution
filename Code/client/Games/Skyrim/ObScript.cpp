#include <TiltedOnlinePCH.h>

#include <Games/Skyrim/ObScript.h>
#include <Games/References.h>

#include <Events/ScriptAnimationEvent.h>

#include <World.h>

// Bound from CommonLibSSE-NG SCRIPT_FUNCTION::LocateScriptCommand, which scans
// kScriptCommandsEnd (0x02E0) entries from the first script command.
static constexpr uint32_t kScriptCommandCount = 0x2E0;

using TParseParameters = bool(const ObScriptParam* apParamInfo, ObScriptData* apScriptData, uint32_t& arOpcodeOffsetPtr, TESObjectREFR* apThisObj, TESObjectREFR* apContainingObj, Script* apScriptObj, ScriptLocals* apLocals, ...);

static ObScriptCommand::TExecute* RealSendAnimationEventExecute = nullptr;
static ObScriptCommand* s_pSaeCommand = nullptr; // kept for a future unhook/restore path

static bool HookSendAnimationEventExecute(const ObScriptParam* apParamInfo, ObScriptData* apScriptData, TESObjectREFR* apThisObj, TESObjectREFR* apContainingObj, Script* apScriptObj, ScriptLocals* apLocals, double& arResult, uint32_t& arOpcodeOffsetPtr)
{
    POINTER_SKYRIMSE(TParseParameters, s_parseParameters, 21910);

    // Parse a copy so the original execute below still sees the unconsumed offset.
    uint32_t opcodeOffset = arOpcodeOffsetPtr;
    char eventNameBuffer[512]{};

    const bool cParsed = s_parseParameters.Get()(apParamInfo, apScriptData, opcodeOffset, apThisObj, apContainingObj, apScriptObj, apLocals, eventNameBuffer);

    const bool cResult = RealSendAnimationEventExecute(apParamInfo, apScriptData, apThisObj, apContainingObj, apScriptObj, apLocals, arResult, arOpcodeOffsetPtr);

    if (cResult && cParsed && apThisObj && eventNameBuffer[0] != '\0')
    {
        spdlog::debug("Console sae captured: ref {:X}, event {}", apThisObj->formID, eventNameBuffer);
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apThisObj->formID, String{}, eventNameBuffer));
    }

    return cResult;
}

static TiltedPhoques::Initializer s_obScriptHooks(
    []()
    {
        // First script command, AE address-library id 361120 (CommonLibSSE-NG include/RE/Offsets.h:481).
        // The sae command is a SCRIPT function ("SendAnimEvent"), not a console command —
        // confirmed via the in-game help listing.
        POINTER_SKYRIMSE(ObScriptCommand, s_firstScriptCommand, 361120);

        ObScriptCommand* pCommands = s_firstScriptCommand.Get();
        if (!pCommands)
        {
            spdlog::error("ObScript: script command table not found, sae sync disabled");
            return;
        }

        for (uint32_t i = 0; i < kScriptCommandCount; ++i)
        {
            ObScriptCommand& command = pCommands[i];

            if (command.pFunctionName && _stricmp(command.pFunctionName, "SendAnimEvent") == 0)
            {
                s_pSaeCommand = &command;
                RealSendAnimationEventExecute = command.pExecuteFunction;
                command.pExecuteFunction = HookSendAnimationEventExecute;

                spdlog::info("ObScript: hooked script command SendAnimEvent (sae)");
                return;
            }
        }

        spdlog::error("ObScript: SendAnimEvent script command not found, sae sync disabled");
        for (uint32_t i = 0; i < kScriptCommandCount; ++i)
        {
            const ObScriptCommand& command = pCommands[i];
            spdlog::debug("ObScript table [{}]: {} ({})", i, command.pFunctionName ? command.pFunctionName : "<null>", command.pShortName ? command.pShortName : "<null>");
        }
    });
