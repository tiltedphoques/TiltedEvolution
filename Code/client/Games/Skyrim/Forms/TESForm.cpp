
#include "TESForm.h"

// NOTE(Force): This should eventually be disabled for release builds.
static TiltedPhoques::Map<int, TiltedPhoques::String> g_extendedEditorMapping;

void Hook_TESForm_SetFormEditorID(TESForm* apSelf, const char* acName)
{
    g_extendedEditorMapping.insert(std::make_pair(apSelf->formID, acName));
}

const char* Hook_TESForm_GetFormEditorID(TESForm* apSelf)
{
    const auto it = g_extendedEditorMapping.find(apSelf->formID);
    return it != g_extendedEditorMapping.end() ? (*it).second.c_str() : (const char*)0x141619370;
}

static TiltedPhoques::Initializer s_TESForm_init([]() {
    TiltedPhoques::Jump(0x140100590, &Hook_TESForm_GetFormEditorID);
    TiltedPhoques::Jump(0x1401008E0, &Hook_TESForm_SetFormEditorID);
});
