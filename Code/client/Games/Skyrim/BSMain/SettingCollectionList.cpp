
#if 0
#include "BSMain/SettingCollectionList.h"
#include "BSMain/Setting.h"

namespace BSMain
{
static void (*SettingCollectionList_Setting_Add)(SettingCollectionList<Setting>*, Setting*);

void Hook_SettingCollectionList_Setting_Add(SettingCollectionList<Setting>* apSelf, Setting* apSetting)
{
    SettingCollectionList_Setting_Add(apSelf, apSetting);

    auto getType = Setting::DataType(apSetting->GetName());

    // MakeNameEx({0:x}, \"{}\", 0x101);
    // spdlog::info("Setting {} (Type: {}) Address: {}", apSetting->GetName(), (int)getType,
    // fmt::ptr(&apSetting->value.i));

    const void* ptr = fmt::ptr(&apSetting->value.i);
    switch (getType)
    {
    case Setting::SETTING_TYPE::ST_FLOAT:
        spdlog::info("MakeFloat({});", ptr);
        break;
    case Setting::SETTING_TYPE::ST_INT:
    case Setting::SETTING_TYPE::ST_UINT:
        spdlog::info("MakeDword({});", ptr);
        break;
    case Setting::SETTING_TYPE::ST_BINARY:
    case Setting::SETTING_TYPE::ST_CHAR:
    case Setting::SETTING_TYPE::ST_UCHAR:
        spdlog::info("MakeByte({});", ptr);
        break;
    case Setting::SETTING_TYPE::ST_NONE:
        return;
    }

    spdlog::info("MakeName({}, \"{}\");", ptr, apSetting->GetName());
}
} // namespace BSMain

static TiltedPhoques::Initializer s_InitSettingCollectionHook([]() {
#if 1
    const VersionDbPtr<uint8_t> addSettingLoc(12990);

    // 0x1401015C0 for gamesettings.
    BSMain::SettingCollectionList_Setting_Add =
        static_cast<decltype(BSMain::SettingCollectionList_Setting_Add)>(addSettingLoc.GetPtr());
    TP_HOOK_IMMEDIATE(&BSMain::SettingCollectionList_Setting_Add, &BSMain::Hook_SettingCollectionList_Setting_Add);
#endif
});
#endif
