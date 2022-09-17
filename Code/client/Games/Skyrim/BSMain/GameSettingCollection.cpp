
#include "GameSettingCollection.h"
#include "Setting.h"
#include "SettingCollection.h"

namespace
{
void Hook_AddSetting_CXX(BSMain::GameSettingCollection* apCollection, BSMain::Setting* apSetting)
{
    // add the original setting
    apCollection->Add(apSetting);
}

TiltedPhoques::Initializer s_InitGameSettingCollection([]() {
    TiltedPhoques::PutCall(0x14000799D, Hook_AddSetting_CXX);
});

} // namespace
