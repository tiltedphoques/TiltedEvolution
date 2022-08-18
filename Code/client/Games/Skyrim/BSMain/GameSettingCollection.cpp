
#include "GameSettingCollection.h"
#include "Setting.h"
#include "SettingCollection.h"

static BSMain::TiltedSetting getest("sTestSetting", true);

namespace
{
void Hook_AddSetting_CXX(BSMain::GameSettingCollection* apCollection, BSMain::Setting* apSetting)
{
    // add the original setting
    apCollection->Add(apSetting);

    BSMain::TiltedSetting::VisitAll([&](BSMain::TiltedSetting* apItem) {
        apCollection->Add(apItem);
    });
}

TiltedPhoques::Initializer s_InitGameSettingCollection([]() {
    TiltedPhoques::PutCall(0x14000799D, Hook_AddSetting_CXX);
});

} // namespace
