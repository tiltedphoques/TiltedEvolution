
#include "INISettingCollectionXX.h"
#include "Setting.h"
#include "SettingCollection.h"

static BSMain::TiltedSetting getest("bEnableST:General", true);

namespace
{
void Hook_AddSetting_CXX2(BSMain::INISettingCollectionXX* apCollection, BSMain::Setting* apSetting)
{
    // add the original setting
    apCollection->Add(apSetting);

    BSMain::TiltedSetting::VisitAll([&](BSMain::TiltedSetting* apItem) {
        apCollection->Add(apItem);
        __debugbreak();
    });
}

BOOL H_WritePrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName)
{
    spdlog::info("AppName: {}, Keyname: {}", lpAppName, lpKeyName);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

TiltedPhoques::Initializer s_InitINISettingCollectionXX([]() {
    TiltedPhoques::PutCall(0x14000B0A1, Hook_AddSetting_CXX2);

    TiltedPhoques::PutCall(0x140D6D7F1, H_WritePrivateProfileStringA);
    TiltedPhoques::Put<uint8_t>(0x140D6D7F1 + 5, 0x90);
});

} // namespace
