#pragma once

struct Script;
struct ScriptLocals;
struct TESObjectREFR;

// Minimal layout of the engine's ObScript (console/script) command table entry.
// Cross-checked against CommonLibSSE-NG RE::SCRIPT_FUNCTION (include/RE/C/CommandTable.h).
struct ObScriptParam
{
    const char* pParamName; // 00
    uint32_t paramType;     // 08
    bool optional;          // 0C
    uint8_t pad0D;          // 0D
    uint16_t pad0E;         // 0E
};

static_assert(sizeof(ObScriptParam) == 0x10);

struct ObScriptData
{
    uint16_t opcode;    // 00
    uint16_t chunkSize; // 02
    uint16_t numParams; // 04
};

static_assert(sizeof(ObScriptData) == 0x6);

struct ObScriptCommand
{
    using TExecute = bool(const ObScriptParam* apParamInfo, ObScriptData* apScriptData, TESObjectREFR* apThisObj, TESObjectREFR* apContainingObj, Script* apScriptObj, ScriptLocals* apLocals, double& arResult, uint32_t& arOpcodeOffsetPtr);

    const char* pFunctionName;  // 00
    const char* pShortName;     // 08
    uint32_t output;            // 10
    uint32_t pad14;             // 14
    const char* pHelpString;    // 18
    bool referenceFunction;     // 20
    uint8_t pad21;              // 21
    uint16_t numParams;         // 22
    uint32_t pad24;             // 24
    ObScriptParam* pParams;     // 28
    TExecute* pExecuteFunction; // 30
    void* pCompileFunction;     // 38
    void* pConditionFunction;   // 40
    bool editorFilter;          // 48
    bool invalidatesCellList;   // 49
    uint16_t pad4A;             // 4A
    uint32_t pad4C;             // 4C
};

static_assert(offsetof(ObScriptCommand, pExecuteFunction) == 0x30);
static_assert(sizeof(ObScriptCommand) == 0x50);
